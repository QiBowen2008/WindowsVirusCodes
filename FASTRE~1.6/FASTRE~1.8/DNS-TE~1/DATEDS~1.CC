/*
 * Similar to perl's hash data type, with a persistent store.
 * For storing null-terminated strings.  We don't store the null
 * terminators in the DB file, for these reasons:
 * 1. Perl scripts that use the file would all have to remove them.
 * 2. The lengths of the strings are stored, so they're unnecessary.
 * 3. To save space.
 * The DB library tries to be smart by mallocing exactly the needed
 * number of bytes for return values.  So we realloc an extra byte,
 * which is normally a no-op because most malloc implementations
 * hand out memory in standard-sized chunks that are generally
 * larger than the size requested.  So we don't confuse the DB library,
 * we have to set the DB_DBT_MALLOC flag, and make the caller responsible
 * for freeing the result.
 *
 * The Db and Dbc classes have private constructors, so we can't
 * subclass them; the child classes' constructors would try to run their
 * parents' constructors.  So the compiler disallows it, and we get by
 * without inheritance.
 *
 * These methods return 0 on success, and nonzero (usually the value of 
 * errno) on failure.  See the DB library documentation.
 *
 * Written by David MacKenzie <djm@web.us.uu.net>
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

#include <sys/types.h>
#include <iostream.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include "DatedStringDb.h"

// DB Memory pool (cache) size in bytes.
#define MPOOL_SIZE (1024*1024*2)

// dbhome is the base if storename is relative, NULL otherwise.
// storename may be NULL to store the strings only in memory.
DatedStringDb::DatedStringDb(const char *dbhome, const char *storename)
{
#if DB_VERSION_MAJOR >= 3
  env = new DbEnv(0);
  env->set_error_stream(&cerr);
  env->set_errpfx(storename ? storename : "DatedStringDb");
  env->set_cachesize(0, MPOOL_SIZE, 0);
  env->open(dbhome, DB_CREATE|DB_INIT_MPOOL|DB_PRIVATE, 0644);
  db = new Db(env, 0);
  db->open(storename, NULL, DB_BTREE, DB_CREATE, 0644);
#elif DB_VERSION_MAJOR == 2
  env = new DbEnv;
  env->set_error_stream(&cerr);
  env->set_errpfx(storename ? storename : "DatedStringDb");
  env->set_error_model(DbEnv::Exception); // Exception or ErrorReturn
  env->set_mp_size(MPOOL_SIZE);
  env->appinit(dbhome, NULL, DB_CREATE|DB_INIT_MPOOL|DB_MPOOL_PRIVATE);
  Db::open(storename, DB_BTREE, DB_CREATE, 0644, env, NULL, &db);
#else
#error Unsupported Berkeley DB version
#endif
}

// Only stores the timestamp if the pointer is nonzero.
int DatedStringDb::put(const char *key, const char *value, time_t *whenp = 0)
{
  static char *tval = NULL;
  static size_t tlen = 0;
  size_t vlen = strlen(value);

  if (whenp) {
    if (tlen < vlen + sizeof(*whenp)) {
      tlen = vlen + sizeof(*whenp);
      if (tval)
	free(tval);
      tval = (char *) malloc(tlen);
      if (!tval)
	return ENOMEM;
    }
    memcpy(tval, whenp, sizeof(*whenp));
    memcpy(tval + sizeof(*whenp), value, vlen);
    value = tval;
    vlen += sizeof(*whenp);
  }

  Dbt dbkey((void *)key, strlen(key)),
     dbdata((void *)value, vlen);

  return db->put(NULL, &dbkey, &dbdata, 0);
}

// If valuep is NULL, just checks for the existence of the record.
// Otherwise, returns a string allocated with malloc,
// which the caller must free.
// Only assumes and returns the timestamp if the pointer is nonzero.
int DatedStringDb::get(const char *key, char **valuep, time_t *whenp = 0)
{
  Dbt dbkey((void *)key, strlen(key)),
     dbdata;
  u_int32_t size;
  int ret;

  if (valuep)
    dbdata.set_flags(DB_DBT_MALLOC);
  ret = db->get(NULL, &dbkey, &dbdata, 0);
  if (ret != 0)
    return ret;

  if (valuep) {
    size = dbdata.get_size();
    if (whenp) {
      *valuep = (char *) dbdata.get_data();
      memcpy(whenp, *valuep, sizeof(*whenp));
      size -= sizeof(*whenp);
      memmove(*valuep, *valuep + sizeof(*whenp), size);
    } else {
      *valuep = (char *) realloc(dbdata.get_data(), size + 1);
      if (!*valuep)
	return ENOMEM;
    }
    (*valuep)[size] = '\0';
  }

  return 0;
}

int DatedStringDb::del(const char *key)
{
  Dbt dbkey((void *)key, strlen(key));

  return db->del(NULL, &dbkey, 0);
}

int DatedStringDb::cursor(Dbc **cursorp)
{
  return db->cursor(NULL, cursorp
#if DB_VERSION_MAJOR >= 3 || DB_VERSION_MINOR >= 6
		    , 0
#endif
		    );
}

DatedStringDb::~DatedStringDb()
{
  close();
  delete env;
}

//////////////////////////////////////////////////////////////////////////////

// Only stores the timestamp if the pointer is nonzero.
int DatedStringDbCursor::put(const char *key, const char *value, u_int32_t flags, time_t *whenp = 0)
{
  static char *tval = NULL;
  static size_t tlen = 0;
  size_t vlen = strlen(value);

  if (whenp) {
    if (tlen < vlen + sizeof(*whenp)) {
      tlen = vlen + sizeof(*whenp);
      if (tval)
	free(tval);
      tval = (char *) malloc(tlen);
      if (!tval)
	return ENOMEM;
    }
    memcpy(tval, whenp, sizeof(*whenp));
    memcpy(tval + sizeof(*whenp), value, vlen);
    value = tval;
    vlen += sizeof(*whenp);
  }

  Dbt dbkey((void *)key, strlen(key)),
     dbdata((void *)value, vlen);

  return dbc->put(&dbkey, &dbdata, flags);
}

// Returns strings allocated with malloc, which the caller must free.
// Only assumes and returns the timestamp if the pointer is nonzero.
int DatedStringDbCursor::get(char **keyp, char **valuep, u_int32_t flags, time_t *whenp = 0)
{
  Dbt dbkey, dbdata;
  u_int32_t size;
  int ret;

  dbkey.set_flags(DB_DBT_MALLOC);
  dbdata.set_flags(DB_DBT_MALLOC);
  ret = dbc->get(&dbkey, &dbdata, flags);
  if (ret != 0)
    return ret;

  size = dbkey.get_size();
  *keyp = (char *) realloc(dbkey.get_data(), size + 1);
  if (!*keyp)
    return ENOMEM;
  (*keyp)[size] = '\0';

  size = dbdata.get_size();
  if (whenp) {
    *valuep = (char *) dbdata.get_data();
    memcpy(whenp, *valuep, sizeof(*whenp));
    size -= sizeof(*whenp);
    memmove(*valuep, *valuep + sizeof(*whenp), size);
  } else {
    *valuep = (char *) realloc(dbdata.get_data(), size + 1);
    if (!*valuep)
      return ENOMEM;
  }
  (*valuep)[size] = '\0';

  return 0;
}
