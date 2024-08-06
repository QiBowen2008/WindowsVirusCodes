/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 * misc.C -
 *     
\*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <assert.h>
#include  <pwd.h>
#include  <sys/types.h>
#include  <unistd.h>

#include  "generic.h"
#include  "Misc.h"
#include  "sarray.h"
#include  "Tree.h"
#include  "treepath.h"
#include  "config.h"

#define  LINE_SIZE  1024

/*=========================================================================
 * Start of Code
\*=========================================================================*/

void   str_replace( char  * in_line, char  * search, const char  * rep )
{
    char   *in_str, *out_str, tmp[ LINE_SIZE ], *pos;
    int  len;
    char  ch;

    //printf( "str_replace( [%s], [%s], [%s] )\n", in_line, search, rep );
    len = strlen( search );

    in_str = in_line;
    out_str = tmp;
    do {
        pos = strstr( in_str, search );
        if  ( pos == NULL )
            break;
        
        /* we copy from in_str till the beginning of the replaced string */
        ch = *pos;
        *pos = 0;
        while  ( *in_str ) {
            *out_str++ = *in_str++;
        }
        *out_str = 0;
        *in_str = ch;
        
        /* we now copy the replaced string instead of the found string */
        strcpy( out_str, rep );
        while  ( *out_str )
            out_str++;

        /* wee jump over the found pattern */
        in_str += len;
    }  while  ( *in_str );
    
    while  ( *in_str ) {
        *out_str++ = *in_str++;
    }
    *out_str = 0;

    strcpy( in_line, tmp );
//    printf( "result = [%s]\n", in_line );
}


void   str_kill_empty_prefix( char  * str )
{
  char  line[ LINE_SIZE ], * old_str;

  old_str = str;
  while  ( ( *str != 0 )  &&  ( ( *str == ' ' )  ||  ( *str == '\t' )
                                ||  ( *str == '\n' ) ) )
    str++;
  strcpy( line, str );
  strcpy( old_str, line );
}


void   str_kill_empty_suffix( char  * str )
{
  char  * pos;
  int  len;

  len = strlen( str );
  pos = str + len - 1;
  while  ( ( len > 0 )  &&  ( ( *pos == ' ' )  ||  ( *pos == '\t' )
                              ||  ( *pos == '\n' ) ) ) {
    *pos = 0;
    pos--;
    len--;
  }
}


void   str_kill_empty_suffix_prefix( char  * str )
{
    str_kill_empty_suffix( str );
    str_kill_empty_prefix( str );
}



void   str_swallow_chars( char  * str, int  count )
{
  char  line[ LINE_SIZE ];

  assert( (int)strlen( str ) >= count );

  strcpy( line, str + count );  
  strcpy( str, line );
}


void  createFileName( const ConfigInfo  & cfg, int id, char  * filename,
                      bool  f_shortname )
{
    char  tmp[ LINE_SIZE ];
    
    if  ( id == 0 ) 
        strcpy( tmp, "bookmarks.html" );
    else
        sprintf( tmp, "book%04d.html", id );
    
    if  ( f_shortname ) 
        filename[ 0 ] = 0;
    else    
        strcpy( filename, cfg.dstFile );
    strcat( filename, tmp );
}


void   str_remove_quotes( char  * str )
{
  if  ( ( str == NULL )  ||  ( strlen( str ) < 2 ) )
    return;

  if  ( ( str[ 0 ] != '\"' )  ||  ( str[ strlen( str ) - 1 ] != '\"' ) )
    return;

  str[ strlen( str ) - 1 ] = 0;
  str_swallow_chars( str, 1 ); 
}


const char  * getHomeDir() 
{
    static char  home[ LINE_SIZE ] = "";
    static bool  f_home_init = false;

    if  ( f_home_init ) 
        return  home;

    home[ 0 ] = 0;
    
    if  ( getenv( "HOME" ) != NULL ) {
        strcpy( home, getenv( "HOME" ) );
    } else {
        struct passwd* pw = getpwuid(getuid());
        if ( pw != NULL  ) 
            strcpy( home, pw->pw_dir );
    }    

    f_home_init = true;

    return   home;
}


bool   getLine( FILE  * fl, char  * line )
{
    char  * res;
    char  tmp[ LINE_SIZE ];

    if  ( feof( fl ) ) 
        return  false;

    do { 
        res = fgets( line, LINE_SIZE, fl );
        if  ( res == NULL )
            return  false;
        res = line;
        while  ( *res != 0 ) {
            if  ( *res == '\n' )
                *res = ' ';
            if  ( *res == '#' ) {
                *res = 0;
                break;
            }
            res++;
        }

        while  ( ( *line == ' ' )  ||  ( *line == '\t' ) ) {
            strcpy( tmp, line + 1 );
            strcpy( line, tmp );
        }
    
        str_replace( line, "~", getHomeDir() );
    }  while  ( *line == 0 );
    str_kill_empty_suffix( line );

    //printf(  "line:[%s]\n", line );

    return  true;
}


void   str_skip( char_ptr_t  & str, const char  * chars )
{
    //printf( "str: [%s], chars: [%s]\n", str, chars );
    while  ( ( *str != 0 )  &&  ( strchr( chars, *str ) != NULL ) ) {
        //printf( "str: [%s], chars: [%s]\n", str, chars );
        str++;
    }
}


const char  * str_pos_unquoted( const char  * line, char  ch )
{
    bool  f_in_str;

    f_in_str = false;
    while  ( *line ) {
        if  ( *line == '"' ) 
            f_in_str = !f_in_str;
        if  ( ( ! f_in_str )  &&  ( *line == ch ) )
            return  line;
        line++;
    }

    return  NULL;
}

void   str_get_quoted_word( char_ptr_t  & str, char  * word )
{
    char  * pos;
    
    assert( *str == '"' );
    str++;
    pos = strchr( str, '"' );
    if  ( pos == NULL ) {
        //printf( "str: [%s]\n", str );
        my_error( "Missing \"" );
    }

    strncpy( word, str, pos - str );
    word[ pos - str ] = 0;
    
    str = pos + 1;
}

bool   str_is_close_brace( char  * line ) 
{
    return( str_pos_unquoted( line, '}' ) != NULL );
}


void        my_error( char        * err )
{
    fflush( stdout );
    fflush( stderr );

    fprintf( stderr, "Error: [%s]\n", err );
    exit( -1 );
}

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 *     
 * misc.C - End of File
\*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
