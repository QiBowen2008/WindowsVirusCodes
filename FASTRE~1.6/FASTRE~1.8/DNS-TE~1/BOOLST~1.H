#ifndef _BOOLSTRING_H
#define _BOOLSTRING_H

/*
 * An efficient container for a C string and a bool.
 * Callers [de]allocate memory for the string.
 * Only implements the operations that we need.
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

#include <stdlib.h>
#include <string.h>
#include <iostream.h>

class BoolString
{
private:
  char *str;
  bool flag;
public:
  BoolString(void) { str=0; flag=false; }
  BoolString(char *astr, bool aflag) {
    str = astr;
    flag = aflag;
  }
  BoolString(const BoolString &other) {
    str = other.str;
    flag = other.flag;
  }
  void set_str(char *astr) { str = astr; }
  char *get_str(void) { return str; }
  void set_flag(bool aflag) { flag = aflag; }
  bool get_flag(void) { return flag; }

  BoolString &operator=(const BoolString &other) {
    str = other.str;
    flag = other.flag;
  }
  int operator<(const BoolString &other) const {
    return strcmp(str, other.str) < 0;
  }
  int operator>(const BoolString &other) const {
    return strcmp(str, other.str) > 0;
  }
  int operator!=(BoolString &other) const {
    return strcmp(str, other.str) != 0;
  }
  int operator==(BoolString &other) const {
    return strcmp(str, other.str) == 0;
  }
};

#endif /* _BOOLSTRING_H */
