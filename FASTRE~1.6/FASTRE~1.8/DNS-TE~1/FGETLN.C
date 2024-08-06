/*
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

/* Some compiler systems don't like empty modules, so ensure something's here.
 * I'm too lazy to exclude this file with configure.
 */
int dummy;

#ifndef HAVE_FGETLN
int getstr(char **lineptr, size_t *n, FILE *stream,
		      char terminator, size_t offset);

/* For BSD4.4 compatibility.  */
char *
fgetln(FILE *stream, size_t *lenp)
{
  static char *p = NULL;
  static size_t psize = 0;
  ssize_t len;

  len = getstr(&p, &psize, stream, '\n', 0);
  *lenp = len;
  return len == -1 ? NULL : p;
}
#endif
