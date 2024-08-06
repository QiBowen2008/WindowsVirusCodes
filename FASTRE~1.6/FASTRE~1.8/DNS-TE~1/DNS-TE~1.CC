/*
 * DNS-Terror.  Part of Fastresolve.
 *
 * Before running this, it's best to run 'unlimit'.
 *
 * Reads IP addresses to resolve from the standard input, one per line.
 * Other stuff on a line after the IP address is ignored.
 *
 * Options:
 * -c adns-conf		ADNS conf string to use instead of /etc/resolv.conf
 *			and the various optional environment variables.
 *			One or more lines in a format like resolv.conf,
 *			with directives: nameserver, domain, search
 *			plus some additional directives:
 *			sortlist, options, clearnameservers, include
 *			One approach is to make an alternate conf file
 *			and use -c "include adns.conf".
 * -d dbfile		Save results to DB file dbfile.  Defaults to
 *			ip2host.db.  If given as the empty string,
 *			the DB is stored in memory, and is lost when the
 *			program exits.
 * -f fields		Skip fields blank-separated fields at the start
 *			of each line before expecting an IP address.
 * -m marksize		Print a notice every marksize input lines.
 * -o			Copy the input lines to the standard output
 *			with IP addresses resolved.
 * -p parallel-queries	Set the size of the query pipeline.
 * -r			Reresolve; do not read in negative cache entries.
 * -s			Sync the DB to disk at each mark.
 * -v			Increase output verbosity.
 *
 * On SIGHUP, closes and reopens the db file (useful if it was rolled).
 * On SIGTERM, closes the db file and exits.
 *
 * Written by David MacKenzie <djm@web.us.uu.net>
 * Thanks to Josh Osborne <stripes@eng.us.uu.net> for ideas and an
 * earlier implementation.
 * Please send comments and bug reports to fastresolve-bugs@web.us.uu.net.
 *
 ******************************************************************************
 *   Copyright 1999 UUNET, an MCI WorldCom company.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 ******************************************************************************
 */

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include <signal.h>
#include <setjmp.h>
#include <adns.h>
#include <zlib.h>
#include <map>
#include <deque>
#include "BoolString.h"
#include "DatedStringDb.h"

extern "C" int getstr(char **lineptr, size_t *n, FILE *stream,
		      char terminator, size_t offset);
#ifndef HAVE_FGETLN
extern "C" char *fgetln(FILE *stream, size_t *lenp);
#endif

// Default maximum number of queries outstanding in the pipeline,
// or with -o, 1/20 the number of buffered log lines.
#define DEFAULT_PARALLEL_QUERIES 1000

// 20-30 is a typical ratio of queried addresses to log lines.
#define COPYLINES_MULTIPLIER 20

// Where to cache the results.
#define DEFAULT_DBFILE "ip2host.db"

typedef map<BoolString, BoolString, less<BoolString> > BoolStringMap;

static char *program_name;

// Degree of verbosity in output.  The more, the messier.
static int verbose = 0;

// Flags set by signal handlers.
static int reopen = 0;
static jmp_buf getback;

void
hup_handler(int)
{
  reopen = 1;
}

void
term_handler(int)
{
  fprintf(stderr, "%s: received terminate signal; exiting\n", program_name);
  longjmp(getback, 1);
}

void
int_handler(int)
{
  fprintf(stderr, "%s: received interrupt signal; exiting\n", program_name);
  longjmp(getback, 1);
}

static void 
fatal_errno(const char *what, int errnoval)
{
  fprintf(stderr, "%s: fatal error: %s: %s\n",
	  program_name, what, strerror(errnoval));
  longjmp(getback, 1);
}

void
set_handlers(void)
{
  struct sigaction act;

  memset(&act, '\0', sizeof act);

  act.sa_handler = hup_handler;
  sigaction(SIGHUP, &act, NULL);

  act.sa_handler = term_handler;
  sigaction(SIGTERM, &act, NULL);

  act.sa_handler = int_handler;
  sigaction(SIGINT, &act, NULL);
}

// Info about one query that's being made.
class LogEntry
{
public:
  adns_query qu;		// ADNS query ID, or NULL.
  char *ipaddr;			// Forward dotted-quad, NUL-terminated.
  char *logbefore, *logafter;	// The rest of the log entry.
  size_t lenbefore, lenafter;	// Lengths; no NUL-termination.
  char buf[1];			// Really longer.  Must be last.  Holds above.
};

typedef deque<LogEntry *> LogEntryQue;

class QueryStats
{
public:
  QueryStats(void) { linesread = cached = submitted = invalid = successful = 0; }
  void print(void);
  long linesread;
  long cached;			// -1 if no cache DB file used.
  long submitted;
  long invalid;
  long successful;
};

void
QueryStats::print(void)
{
  fprintf(stderr, "%ld lines read.\n", linesread);
  fprintf(stderr, "%ld (%.2f%%) invalid addresses.\n",
	  invalid, linesread ? ((100.0 * invalid) / (1.0 * linesread)) : 0.0);

  if (cached >= 0)
    fprintf(stderr, "%ld (%.2f%%) cache hits from the DB file.\n",
	    cached, linesread ? ((100.0 * cached) / (1.0 * linesread)) : 0.0);

  fprintf(stderr, "%ld (%.2f%%) addresses were queried with DNS;\n",
	  submitted, linesread ? ((100.0 * submitted) / (1.0 * linesread)) : 0.0);
  fprintf(stderr, "%ld (%.2f%%) of those queries were successful.\n",
	  successful, submitted ? ((100.0 * successful) / (1.0 * submitted)) : 0.0);
}

// Maximum bytes in an ASCII IPv4 address.
#define MAX_IP_LEN 15
// The size of "zzz.yyy.xxx.www.in-addr.arpa\0"
#define MAX_PTR_SIZE (MAX_IP_LEN + 14)
// Define to do pedantic checking that domain has a valid format.
// #define CHECK_PTR_SYNTAX

// If domain contains "www.xxx.yyy.zzz" then put in ptr
// "zzz.yyy.xxx.www.in-addr.arpa".
// ptr must be at least MAX_PTR_SIZE bytes long.
// Return 1 if ok, 0 if domain is not an IPv4 address.
//
// We leave off the final "." because the returned answers lack it,
// and we need to compare with them, and this is more efficient than
// adding a "." to the end of each of them.
// Moreover, we're not passing the adns_qf_search flag to
// adns_submit(), so we're not searching anyway.
int
domptr(const char *domain, char *ptr)
{
  const char *inaddr = ".in-addr.arpa";
  size_t domsize = strlen(domain);
  const char *d;
  const char *numstart, *numend = NULL;
  char *p = ptr;
#ifdef CHECK_PTR_SYNTAX
  int octets = 0, dots = 0, val;
#endif

  if (domsize + sizeof(inaddr) > MAX_PTR_SIZE) {
    return 0;
  }
  for (d = domain + domsize - 1; d >= domain; d--) {
    if (isdigit(*d)) {
      if (!numend)
	numend = d;
    } else if (*d == '.') {
      if (numend) {
#ifdef CHECK_PTR_SYNTAX
	val = 0;
#endif
	for (numstart = d + 1; numstart <= numend; ++numstart) {
#ifdef CHECK_PTR_SYNTAX
	  val = val * 10 + *numstart - '0';
#endif
	  *p++ = *numstart;
	}
	numend = NULL;
#ifdef CHECK_PTR_SYNTAX
	if (val > 255)
	  return 0;
	++octets;
#endif
      }
      *p++ = *d;
#ifdef CHECK_PTR_SYNTAX
      ++dots;
#endif
    } else {
      return 0;
    }
  }

  if (numend) {
#ifdef CHECK_PTR_SYNTAX
    val = 0;
#endif
    for (numstart = d + 1; numstart <= numend; ++numstart) {
#ifdef CHECK_PTR_SYNTAX
      val = val * 10 + *numstart - '0';
#endif
      *p++ = *numstart;
    }
#ifdef CHECK_PTR_SYNTAX
    if (val > 255)
      return 0;
    ++octets;
#endif
  }
#ifdef CHECK_PTR_SYNTAX
  if (octets != 4 || dots != 3)
    return 0;
#endif
  strcpy(p, inaddr);
  return 1;
}

#if 0
void
print_map(BoolStringMap &reslist, bool all)
{
  BoolStringMap::iterator it;
  BoolString k, v;

  fprintf(stderr, "MAP:\n");
  for (it = reslist.begin(); it != reslist.end(); it++) {
    k = (*it).first;
    v = (*it).second;
    if (all || strcmp(v.get_str(), "?"))
      fprintf(stderr, "%s=%s\n", k.get_str(), v.get_str());
  }
}
#endif

enum submission { sb_invalid, sb_cached, sb_known, sb_pending, sb_submitted };

enum submission
submit_query(adns_state ads, BoolStringMap &reslist, LogEntry *lp)
{
  int r;
  adns_query qu;
  char rev[MAX_PTR_SIZE], *ipaddr, *data;

  if (!domptr(lp->ipaddr, rev)) {
    if (verbose)
      fprintf(stderr, "%s invalid\n", lp->ipaddr);
    return sb_invalid;
  }

  BoolString key(lp->ipaddr, false), value;
  BoolStringMap::iterator it = reslist.find(key);
  if (it != reslist.end()) {
    value = (*it).second;
    data = value.get_str();
    if (data[0] == '?' && data[1] == '\0') {
      if (verbose > 1)
	fprintf(stderr, "%s pending\n", lp->ipaddr);
      return sb_pending;
    }
    if (value.get_flag()) {
      if (verbose > 1)
	fprintf(stderr, "%s known\n", lp->ipaddr);
      return sb_known;
    } else {
      if (verbose > 1)
	fprintf(stderr, "%s cached\n", lp->ipaddr);
      return sb_cached;
    }
  }
  
  r = adns_submit(ads, rev, adns_r_ptr_raw,
		  (enum adns_queryflags)
		  (adns_qf_quoteok_cname|adns_qf_quoteok_anshost), lp, &qu);
  if (r)
    fatal_errno("adns_submit", r);
  if (verbose)
    fprintf(stderr, "%s submitted\n", lp->ipaddr);

  lp->qu = qu;

  ipaddr = strdup(lp->ipaddr);
  if (ipaddr == NULL)
    fatal_errno("malloc", errno);

  BoolString k(ipaddr, false), v("?", true);
  reslist[k] = v;

  return sb_submitted;
}

// Record the resource record(s) we got back.
// Do not free the return value, which is used in reslist.
char *
process_answer(adns_answer *ans, char *ipaddr, BoolStringMap &reslist)
{
  const char *rrtn, *fmtn;
  char *ptr;
  int len;
  adns_status ri;

  ri = adns_rr_info(ans->type, &rrtn, &fmtn, &len, 0, 0);
  if (verbose)
    fprintf(stderr, "%s %s; nrrs=%d ",
	     ipaddr,
	     adns_strerror(ans->status),
	     ans->nrrs);
  if (ans->nrrs) {
    ptr = *ans->rrs.str;
    if (verbose)
      fprintf(stderr, "%s\n", ptr);
  } else {
    ptr = "";
    if (verbose)
      putc('\n', stderr);
  }

  ptr = strdup(ptr);
  if (ptr == NULL)
    fatal_errno("malloc", errno);

  // Update the value from "?".
  BoolString key(ipaddr, false), oldvalue;
  BoolStringMap::iterator it = reslist.find(key);
  assert(it != reslist.end());
  key = (*it).first;		// Don't lose that malloc'd string.
  oldvalue = (*it).second;
  char *data = oldvalue.get_str();
  assert(data[0] == '?' && data[1] == '\0');
  assert(oldvalue.get_flag());

  BoolString value(ptr, true);
  reslist[key] = value;

  return ptr;
}

// Read fields space-separated fields from fp, and return the result.
// Store in *lenp the number of characters read, not including the
// null terminator.
char *
read_fields(FILE *fp, int fields, size_t *lenp)
{
  static char *p = NULL;
  static size_t psize = 0;
  ssize_t nread;
  size_t off;

  off = 0;
  while (fields-- > 0 &&
	 (nread = getstr(&p, &psize, fp, ' ', off)) > 0) {
    off += nread;
  }
  *lenp = off;
  return off == 0 ? NULL : p;
}

// Return the IP address of the next log entry, NUL terminated.
// The result is in static storage that will be overwritten
// by the next call.
// Return NULL on EOF.
// If save_line is true, save the contents of the line in the returned
// structure.  If skip_fields is nonzero, there are that many
// space-separated fields before the IP address.
LogEntry *
read_ipaddr(FILE *fp, bool save_line, int skip_fields)
{
  static char ipa[MAX_IP_LEN + 1];
  char *before;
  size_t after_len = 0, before_len = 0;
  char *p = ipa, *after = "", *to, *from, *end;
  int c;
  LogEntry *lp;

  if (skip_fields)
    before = read_fields(fp, skip_fields, &before_len);

  while ((c = getc(fp)) != EOF
	 && !isspace(c)
	 && p - ipa < MAX_IP_LEN) {
    if (c)			// Guard against corruption (NUL bytes).
      *p++ = c;
  }
  *p = '\0';
  if (c == EOF)
    // Note that we throw away any IP address that is the last thing
    // in the input stream.  It must be followed by something
    // (a newline or two other characters will do) in order to be returned.
    return NULL;

  // N.B. BSD fgetln() does not NUL terminate.
  if (c != '\n' && (after = fgetln(fp, &after_len)) == NULL)
    return NULL;

  lp = (LogEntry *)
    malloc(sizeof(LogEntry)
	   + (save_line ? before_len : 0) // logbefore
	   + p - ipa		// ipaddr (buf already has 1 byte for NUL)
	   + (save_line ? after_len + 1 : 0) // logafter
	   );
  if (lp == NULL)
    fatal_errno("malloc", errno);

  lp->qu = 0;

  // Point ipaddr, logafter, logbefore to data in buf.
  to = lp->buf;  

  // Copy the IP address into the LogEntry.
  for (lp->ipaddr = to, from = ipa; *from;)
    *to++ = *from++;
  *to = '\0';

  // Copy the rest of the line into the LogEntry, if requested.
  if (save_line) {
    lp->logafter = ++to;
    *to++ = c;
    end = after + after_len;	// Sentinel for speed.
    for (from = after; from != end;)
      *to++ = *from++;
    lp->lenafter = after_len + 1;

    if (before_len) {
      lp->logbefore = ++to;
      end = before + before_len;	// Sentinel again.
      for (from = before; from != end;)
	*to++ = *from++;
    }
    lp->lenbefore = before_len;
  }

  return lp;
}

// Read DB entries created by earlier runs into reslist,
// marked as 'not new' (false).
// If skipempty is true, do not read in negative cache entries.
void
read_db(DatedStringDb *table, BoolStringMap &reslist, bool skipempty)
{
  DatedStringDbCursor *iterator = new DatedStringDbCursor(table);
  char *key, *data;
  time_t when;
  int inserted = 0;

  while (iterator->get(&key, &data, DB_NEXT, &when) == 0)
    {
      if (*data || !skipempty) {
	BoolString k(key, false), v(data, false);
	reslist[k] = v;
	++inserted;
      }
    }

  iterator->close();

  if (verbose)
    fprintf(stderr, "read %d addresses from DB file\n", inserted);
}

// Write out new entries to the DB file.
void
store_db(DatedStringDb *table, BoolStringMap &reslist)
{
  BoolStringMap::iterator it;
  char *key, *data;
  time_t when;
  int added = 0;

  time(&when);

  for (it = reslist.begin(); it != reslist.end(); it++) {
    BoolString ipaddr = (*it).first, value = (*it).second;
    if (value.get_flag()) {
      if (verbose > 1)
	fprintf(stderr, "%s new address\n", ipaddr.get_str());
      try
	{
	  table->put(ipaddr.get_str(), value.get_str(), &when);
	}
      catch (DbException &dbe)
	{
	  fprintf(stderr, "DB error storing (%s,%s): %s\n",
		  ipaddr.get_str(), value.get_str(), dbe.what());
	}
      ++added;
      value.set_flag(false);
    }
  }

  table->sync();

  if (verbose)
    fprintf(stderr, "added %d addresses to DB file\n", added);
}

void
usage(char *prog)
{
  fprintf(stderr,
	  "Usage: %s [-v...] [-orsz] [-d db-file] [-c adns-conf-str] [-m mark-size] [-p parallel-queries] [-f skip-fields]\n",
	  prog);
  exit(1);
}

int 
main(int argc, char *const *argv)
{
  adns_state ads;
  int r;
  adns_initflags aflags;
  int outstanding = 0;
  size_t qsize;
  gzFile zout = NULL;
  adns_query qu;
  adns_answer *ans;
  DatedStringDb *resolved;
  BoolStringMap reslist;
  LogEntryQue lq;
  char *dbhome = NULL;
  char *dbfile = DEFAULT_DBFILE;
  char *adnsconf = NULL;
  long marksize = 0, lastmark = -1;
  bool syncmark = false;
  bool copylines = false;
  bool reresolve = false;
  int skip_fields = 0;
  int parallel_queries = DEFAULT_PARALLEL_QUERIES;
  QueryStats stats;
  LogEntry *lp;
  char *dom;

  program_name = argv[0];

  while ((r = getopt(argc, argv, "c:d:f:m:op:rsvz")) != -1) {
    switch (r) {
    case 'c':
      adnsconf = optarg;
      break;

    case 'd':
      dbfile = optarg;
      break;

    case 'f':
      skip_fields = atoi(optarg);
      break;

    case 'm':
      marksize = atol(optarg);
      break;

    case 'o':
      copylines = true;
      break;

    case 'p':
      parallel_queries = atoi(optarg);
      break;

    case 'r':
      reresolve = true;
      break;

    case 's':
      syncmark = true;
      break;

    case 'v':
      verbose++;
      break;

    case 'z':
      zout = gzdopen(1, "wb");
      if (!zout) {
	fprintf(stderr, "%s: fatal error: %s: %s\n",
		program_name, "gzdopen", strerror(errno));
	exit(1);
      }
      break;

    default:
      usage(argv[0]);
      break;
    }
  }

  // We get back here when terminating abnormally.
  r = setjmp(getback);
  if (r == 1)
    goto out;
  else if (r == 2) {
    if (copylines) {
      if (zout) {
	if (lp->lenbefore)
	  gzwrite(zout, lp->logbefore, lp->lenbefore);
	gzputs(zout, lp->ipaddr);
	gzwrite(zout, lp->logafter, lp->lenafter);
      } else {
	if (lp->lenbefore)
	  fwrite(lp->logbefore, lp->lenbefore, 1, stdout);
	fputs(lp->ipaddr, stdout);
	fwrite(lp->logafter, lp->lenafter, 1, stdout);
      }
    }
    goto timed_out;
  }

  if (verbose > 2)
    aflags = (adns_initflags) adns_if_debug;
  else
    aflags = (adns_initflags) 0;
  if (adnsconf)
    r = adns_init_strcfg(&ads, aflags, stderr, adnsconf);
  else
    r = adns_init(&ads, aflags, stderr);
  if (r)
    fatal_errno("adns_init", r);

  try
    {
      resolved = new DatedStringDb(dbhome, *dbfile ? dbfile : NULL);

      set_handlers();

      if (*dbfile)
	read_db(resolved, reslist, reresolve);
      else
	stats.cached = -1;
    }
  catch (DbException &dbe)
    {
      fprintf(stderr, "DB error opening %s: %s\n",
	      *dbfile ? dbfile : "memory DB", dbe.what());
      exit(1);
    }

  if (copylines)
    parallel_queries *= COPYLINES_MULTIPLIER;

  while ((lp = read_ipaddr(stdin, copylines, skip_fields))) {
    ++stats.linesread;
    if (marksize && stats.linesread % marksize == 0) {
      if (copylines)
	fprintf(stderr, "On line %ld, %d queries outstanding, %d lines buffered\n",
		stats.linesread, outstanding, lq.size());
      else
	fprintf(stderr, "On line %ld, %d queries outstanding\n",
		stats.linesread, outstanding);
      if (syncmark && *dbfile)
	store_db(resolved, reslist);
    }

    if (reopen && *dbfile) {
      if (verbose) {
	fprintf(stderr, "%s: received hangup signal; reopening DB file\n", program_name);
      }
      try
	{
	  delete resolved;
	  resolved = new DatedStringDb(dbhome, dbfile);
	}
      catch (DbException &dbe)
	{
	  fprintf(stderr, "DB error reopening %s: %s\n",
		  dbfile, dbe.what());
	  exit(1);
	}
      store_db(resolved, reslist);
      reopen = 0;
    }

    // When doing copylines:
    // We need to save the log files for all entries,
    // not just those that we submit.  The log line must be
    // in the deque.
    //
    // When we read a log file entry to enqueue it, the following
    // situations are possible:
    // 0. The IP address field is syntactically invalid.
    //    There is no answer cached in the map.
    //    We don't need a query ID (assume it's 0).
    // 1. The answer is cached in the map, and is not "?".
    //    We don't need a query ID (assume it's 0).
    // 2. The answer is cached in the map, and is "?", meaning
    //    that the query is in process for some earlier entry.
    //    By the time we dequeue this entry, the earlier entry will
    //    have been written out, so the answer will be in the map.
    //    We don't need a query ID (assume it's 0).
    // 3. The answer is not in the map.  We need to submit a
    //    new query for it and save the query ID.
    //
    // When we dequeue a log file entry, the following
    // situations are possible:
    // 0. The query ID is 0 and there's no answer in the map,
    //    so the IP address is syntactically invalid.
    //    We can write out the line.
    // 1. The query ID is 0.  We know the answer is in the map,
    //    and is not "?".  We can write out the line.
    // 2. The query ID is not 0.  The query is in process.
    //    We need to wait on the query ID for it.

    switch (submit_query(ads, reslist, lp)) {
    case sb_invalid:
      ++stats.invalid;
      break;

    case sb_pending:
      break;

    case sb_known:
      break;

    case sb_cached:
      ++stats.cached;
      break;

    case sb_submitted:
      ++outstanding;
      ++stats.submitted;
      break;
    }

    if (copylines)
      lq.push_back(lp);

    // The goal is to keep the query pipeline full, so only pick off
    // one answer.

    if (copylines)
      assert(lq.size() <= parallel_queries);
    else
      assert(outstanding <= parallel_queries);
    if (copylines ? (lq.size() == parallel_queries) : (outstanding == parallel_queries)) {
      if (copylines) {
	lp = lq[0];
	lq.pop_front();
	qu = lp->qu;
      } else {
	qu = 0;
      }
      if (qu || !copylines) {
	// It would be best if the answer were always ready here, so the
	// adns_wait call wouldn't block.  That would happen if the time
	// it takes to generate a full pipeline of queries were equal to
	// (or greater than, though that wouldn't help) the maximum time it
	// takes to get back a response (or timeout).  Otherwise, the
	// pipeline will stall.
	//
	// For example, with the default (-p 1000) for -o of 20,000 lines
	// buffered, there may be about 800 queries outstanding.
	// On a Pentium III/500, we can generate about 1400 queries per second.
	// If we set udpmaxretries:4 for an 8 second timeout, then we
	// may generate no more than 800/8=100 queries per second to
	// avoid all pipeline stalls on timeouts.  8*1400=11200; round to 12000.
	// -p 12000 will give better performance assuming you have enough RAM.
	//
	// Even if you get the main loop optimal, the pipeline will
	// probably stall during the drain time at the end.
	r = adns_wait(ads, &qu, &ans, (void **) &lp);
	if (r)
	  fatal_errno("adns_wait", r);
	dom = process_answer(ans, lp->ipaddr, reslist);
	if (*dom)
	  ++stats.successful;
	--outstanding;
      } else {
	// It's already in the list.
	BoolString key(lp->ipaddr, false), value;
	BoolStringMap::iterator	it = reslist.find(key);
	if (it != reslist.end()) {
	  value = (*it).second;
	  dom = value.get_str();
	} else {
	  // For a syntactically invalid IP address, there's no answer.
	  // Print it unchanged.
	  dom = lp->ipaddr;
	}
	ans = NULL;
      }
      if (copylines) {
	if (zout) {
	  if (lp->lenbefore)
	    gzwrite(zout, lp->logbefore, lp->lenbefore);
	  gzputs(zout, *dom ? dom : lp->ipaddr);
	  gzwrite(zout, lp->logafter, lp->lenafter);
	} else {
	  if (lp->lenbefore)
	    fwrite(lp->logbefore, lp->lenbefore, 1, stdout);
	  fputs(*dom ? dom : lp->ipaddr, stdout);
	  fwrite(lp->logafter, lp->lenafter, 1, stdout);
	}
      }
      if (ans)
	free(ans);
      free(lp);
    }
  }

  if (verbose || marksize) {
    fprintf(stderr, "Read last line %ld, %d queries outstanding\n",
	    stats.linesread, outstanding);
    if (syncmark && *dbfile)
      store_db(resolved, reslist);
  }

  // Pick up the stragglers; let the pipeline drain.
  if (marksize) {
    marksize = outstanding / 10;
    if (!marksize)
      marksize = 1;
  }
  while (copylines ? ((qsize = lq.size()) > 0) : (outstanding > 0)) {
    if (marksize && outstanding % marksize == 0 && outstanding != lastmark) {
      lastmark = outstanding;
      if (copylines)
	fprintf(stderr, "%d queries outstanding, %d lines buffered\n",
		outstanding, (int) qsize);
      else
	fprintf(stderr, "%d queries outstanding\n", outstanding);
    }
    // Yes, the body of this loop is identical to the inner loop above.
    // I can't think of an easy way to roll them together, given all
    // the variables they use.
    if (copylines) {
      lp = lq[0];
      lq.pop_front();
      qu = lp->qu;
    } else {
      qu = 0;
    }
    if (qu || !copylines) {
      r = adns_wait(ads, &qu, &ans, (void **) &lp);
      if (r)
	fatal_errno("adns_wait", r);
      dom = process_answer(ans, lp->ipaddr, reslist);
      if (*dom)
	++stats.successful;
      --outstanding;
    } else {
      // It's already in the list.
      BoolString key(lp->ipaddr, false), value;
      BoolStringMap::iterator it = reslist.find(key);
      if (it != reslist.end()) {
	value = (*it).second;
	dom = value.get_str();
      } else {
	dom = lp->ipaddr;
      }
      ans = NULL;
    }
    if (copylines) {
      if (zout) {
	if (lp->lenbefore)
	  gzwrite(zout, lp->logbefore, lp->lenbefore);
	gzputs(zout, *dom ? dom : lp->ipaddr);
	gzwrite(zout, lp->logafter, lp->lenafter);
      } else {
	if (lp->lenbefore)
	  fwrite(lp->logbefore, lp->lenbefore, 1, stdout);
	fputs(*dom ? dom : lp->ipaddr, stdout);
	fwrite(lp->logafter, lp->lenafter, 1, stdout);
      }
    }
    if (ans)
      free(ans);
    free(lp);
  }

 timed_out:
  if (*dbfile)
    store_db(resolved, reslist);

 out:
  if (zout)
    gzclose(zout);

  try
    {
      resolved->close();
    }
  catch (DbException &dbe)
    {
      fprintf(stderr, "DB error closing %s: %s\n",
	      *dbfile ? dbfile : "memory DB", dbe.what());
    }

  adns_finish(ads);

  stats.print();

  exit (0);
}
