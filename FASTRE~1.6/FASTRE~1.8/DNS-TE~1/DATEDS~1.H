#ifndef _DATEDSTRINGDB_H
#define _DATEDSTRINGDB_H

/*
 * Similar to perl's hash data type, with a persistent store.
 * For storing null-terminated strings.
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

#define __WANT_DB2__
#include <db_cxx.h>

class DatedStringDb
{
 public:
  DatedStringDb(const char *dbhome, const char *storename);
  int put(const char *key, const char *value, time_t *whenp = 0);
  int get(const char *key, char **valuep, time_t *whenp = 0);
  int del(const char *key);
  int cursor(Dbc **cursorp);
  int sync(void) { return db->sync(0); }
  int close(void) {
    int ret = db->close(0);
#if DB_VERSION_MAJOR >= 3
    env->close(0);
#endif
    return ret;
  };
  ~DatedStringDb();

 private:
  DbEnv *env;
  Db *db;
};

class DatedStringDbCursor
{
 public:
  DatedStringDbCursor(DatedStringDb *sdb) { sdb->cursor(&dbc); };
  int put(const char *key, const char *value, u_int32_t flags, time_t *whenp = 0);
  int get(char **keyp, char **valuep, u_int32_t flags, time_t *whenp = 0);
  int del(void) { return dbc->del(0); }
  int close(void) { return dbc->close(); };

 private:
  Dbc *dbc;
};

#endif /* _DATEDSTRINGDB_H */
