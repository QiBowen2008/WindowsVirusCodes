/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 * misc.h -
 *     
\*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#ifndef  __MISC__H
#define  __MISC__H

typedef  char  * char_ptr_t;

void   str_replace( char  * in_line, char  * search, const char  * rep );
void   str_kill_empty_suffix( char  * str );
void   str_kill_empty_prefix( char  * str );

void   str_kill_empty_suffix_prefix( char  * str );

void   str_swallow_chars( char  * str, int  count );
void   str_remove_quotes( char  * str );
bool   str_is_close_brace( char  * line );
const char  * str_pos_unquoted( const char  * line, char  ch );
void   str_skip( char_ptr_t  & str, const char  * chars );
void   str_get_quoted_word( char_ptr_t  & str, char  * word );

bool   getLine( FILE  * fl, char  * line );
const char  * getHomeDir();


class  ConfigInfo;

void  createFileName( const ConfigInfo  & cfg, int id, char  * filename,
                      bool  f_shortname );



inline int   stricmp(const char *s1, const char *s2)
{
  return  strcasecmp( s1, s2 );
}

inline int   strnicmp(const char *s1, const char *s2, size_t  n )
{
  return  strncasecmp( s1, s2, n );
}

inline bool  strIsPrefix( char  * prefix, char  * line )
{
  return  strnicmp( prefix, line, strlen( prefix ) ) == 0;
}



#else   /* __MISC__H */
#error  Header file misc.h included twice
#endif  /* __MISC__H */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 *     
 * misc.h - End of File
\*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
