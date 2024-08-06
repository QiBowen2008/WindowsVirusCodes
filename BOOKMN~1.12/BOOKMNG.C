/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 * bookmng.C -
 *    Build a bunch of html files that contains in an hierarchical manner
 * bookmars for html.
 * 
 * Program is copyrighted under the GPL.
\*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#include  <stdlib.h>
#include  <stdio.h>
#include  <string.h>
#include  <assert.h>
#include  <time.h>

#include  "generic.h"
#include  "sarray.h"
#include  "Tree.h"
#include  "Misc.h"
#include  "treepath.h"
#include  "config.h"
#include  "VERSION.h"


/*=========================================================================
 * Start of Code
\*=========================================================================*/


static bool   strIsPrefixAndRemove( char  * prefix, char  * line )
{
    int  len;

    if  ( ! strIsPrefix( prefix, line ) ) 
        return  false;

    len = strlen( prefix );
    if  ( ( line[ len ] != ' ' )  &&  ( line[ len ] != '\t' )  
          &&  ( line[ len ] != '\n' ) ) 
        return  false;

    str_swallow_chars( line, strlen( prefix ) );
    str_kill_empty_prefix( line );
 
    return  true;
}
 

static bool   strIsPrefixFlag( char  * prefix, char  * line, bool  & flag )
{
    int  len, args, num;

    if  ( ! strIsPrefix( prefix, line ) ) 
        return  false;

    len = strlen( prefix );
    if  ( ( line[ len ] != ' ' )  &&  ( line[ len ] != '\t' )  
          &&  ( line[ len ] != '\n' ) ) 
        return  false;

    str_swallow_chars( line, strlen( prefix ) );
    str_kill_empty_prefix( line );

    args = sscanf( line, "%d", &num );
    if  ( args < 0 ) {
        fprintf( stderr, "Unable to decipher argument: [%s]\n", prefix );
        exit( -1 );
    }
    flag = ( num != 0 );

    return  true;
}
 

static  void   read_script_file( char   * fl_name, ConfigInfo  & cfg )
{
    FILE  * fl;
    char  line[ LINE_SIZE ], oldLine[ LINE_SIZE ];
    bool  f_verbose, f_credit;

    f_verbose = f_credit = false;

    fl = fopen( fl_name, "rt" );
    if  ( fl == NULL ) {
        fprintf( stderr, "Me sad. Open file [%s] no can do!\n", fl_name );
        exit( -1 );
    }

    while  ( getLine( fl, line ) ) {
        strcpy( oldLine, line );

        if  ( strIsPrefixAndRemove( "source", line ) ) {
            strcpy( cfg.srcFile, line );
            continue;
        }
        if  ( strIsPrefixAndRemove( "dest", line ) ) {
            strcpy( cfg.dstFile, line );
            continue;
        }
        if  ( strIsPrefixAndRemove( "prefix", line ) ) {
            strcpy( cfg.prefixFile, line );
            continue;
        }
        if  ( strIsPrefixFlag( "give_credit_to_bookmng", line, f_credit ) ) {
            cfg.f_give_credit = f_credit;
            continue;
        }
        if  ( strIsPrefixFlag( "verbose", line, f_verbose ) ) {
            cfg.f_verbose = f_verbose;
            continue;
        }
        if  ( strIsPrefixAndRemove( "kill", line ) ) {
            str_remove_quotes( line );
            cfg.addKillString( line );
            continue;
        }
        if  ( strIsPrefixAndRemove( "filename", line ) ) {
            TreePath path;

            path.init( line, fl );
            cfg.addAltFileName( path );
            
            continue;
        }

        fprintf( stderr, "Error: I am shattered to bits! I dont know how to"
                 " handle this line:\n\t%s\n", oldLine );
        exit( -1 );
        //give_credit_to_bookmng 1

        //  char  strSource[ LINE_SIZE ], strDest[ LINE_SIZE ], strPrefix[ LINE_SIZE ];
        // bool  f_give_credit;
      
    }

    fclose( fl );
}


int   main( int  argc, char  ** argv )
{
    ConfigInfo  cfg;
    int  count;
    char  main_file[ LINE_SIZE ];

    printf( "bookmng%s- Create html files from netscape bookmarks file\n",
            VERSION );
    printf( "Written by Sariel Har-Peled - bugs are us!\n\n" );
    if  ( argc != 2 ) {
        fprintf( stderr, "bookmng [bookmarks.cfg]\n" );
        return  -1;
    }
    read_script_file( argv[ 1 ], cfg );
    if  ( cfg.srcFile[ 0 ] == 0 ) {
        fprintf( stderr, "Error: You tell me your bookmarks"
                 " input file, and I will create it!\n" );
        exit( -1 );
    }

    Tree  tree( cfg );
   
    tree.construct( cfg.srcFile );
    //tree.dump();
    count = tree.destroyByKillStrings();

    tree.computeFiles();
    tree.computeFileNames();

    tree.registerAltFileNames();

    if  ( ! cfg.f_give_credit ) 
        printf( "What! Me work so hard on program and me no credit? Me sad.\n" );

    tree.createFiles();
    tree.createMenuFile(); 

    if  ( count > 0 ) 
        printf( "Info: %d entries killed by kill-strings in config file\n",
                count );    
    printf( "      %d files were created\n", tree.getNumberFilesCreated() );
    tree.getMainFileName( main_file );
    printf( "      root file is [%s]\n", main_file );

    printf( "\n" );

    return  0;
}

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 *     
 * bookmng.C - End of File
 \*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

