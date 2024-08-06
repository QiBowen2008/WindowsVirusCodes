/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 * treepath.C -
 *     
\*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#include  <stdlib.h>
#include  <stdio.h>
#include  <assert.h>
#include  <string.h>

#include  "generic.h"
#include  "sarray.h"
#include  "Tree.h"
#include  "Misc.h"
#include  "treepath.h"

/*--- Constants ---*/


/*--- Start of Code ---*/


void  TreePath::init( char  * line, FILE  * fl )
{
    char  buf[ LINE_SIZE ];
    line_type  word;
    char  * start_brace, * end_brace, * str;

    strcpy( buf, line );
    while  ( ! str_is_close_brace( line ) ) {
        if  ( ! getLine( fl, line ) ) {
            fprintf( stderr, "Unexpected end of file!\n" );
            exit( -1 );
        }
        strcat( buf, line );
    }
    
    start_brace = (char *)str_pos_unquoted( buf, '{' );
    end_brace = (char *)str_pos_unquoted( buf, '}' );
    
    if  ( ( start_brace == NULL )  ||  ( end_brace == NULL ) )
        my_error( "Either { or } missing in line!\n" );

    str = start_brace + 1;
    while  ( *str != '}' ) {
        str_skip( str, " \t\n," );
        if  ( *str == '}' )
            break;
        if  ( *str != '"' ) {
            fprintf( stderr, "Unexpected character encountered: [%c]\n", 
                     *str );
            exit( -1 );
        }
        
        str_get_quoted_word( str, word );
        
        push( word );
    }

    if  ( str != end_brace ) 
        my_error( "Unbalanced braces!\n" );
    str++;
    
    strcpy( new_file_name, str );
    str_kill_empty_suffix_prefix( new_file_name );

    if  ( strlen( new_file_name ) <= 0 ) 
        my_error( "No filename given for filename!" );
}



    /*
filename { "Bookmarks", 
           "Computers", 
           "Computer Science",  
           "Computatoional Geometry" }  cg_links.html
    */




/* treepath.C - End of File ------------------------------------------*/
