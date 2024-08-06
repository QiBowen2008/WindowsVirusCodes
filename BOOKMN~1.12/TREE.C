/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 * Tree.C -
 *     
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


/*=========================================================================
 * Start of Code
\*=========================================================================*/

static void  dumpIntoFile( FILE  * fl, const char  * srcFile )
{
    FILE  * fl_in;
    char  * pos;
    char  line[ LINE_SIZE ];

    fl_in = fopen( srcFile, "rt" );
    if  ( fl_in == NULL ) {
        fprintf( stderr, "Unable to open prefix file!\n\tFile: [%s]\n", 
                 srcFile );
        exit( -1 );
    }

    while  ( !feof( fl_in ) ) {
        pos = fgets( line, LINE_SIZE, fl_in );
        if  ( pos == NULL ) 
            break;

        fputs( line, fl );
    }

    fclose( fl_in ); 
}


static void  printHTMLStart( ConfigInfo  & cfg, 
                             FILE  * fl, const char  * title )
{
    fprintf( fl, 
             "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n" 
             "<HTML>\n"
             "<head>"
             "<TITLE>\n"
             "%s\n"
             "</TITLE>\n"
             "</head>\n", title );

    if  ( cfg.prefixFile[ 0 ] != 0 ) {
        dumpIntoFile( fl, cfg.prefixFile );
    } else {
        fprintf( fl, "<body>" );
    }
}

static void  handle_line_1( char  * in_line )
{
    char  line[ LINE_SIZE ], *pos, *pos1, *pos2, *pos3;

    strcpy( line, in_line );
    pos = strstr( line, "A HREF=" );
    if  ( pos == NULL ) 
        return;
    pos1 = strchr( pos, '\"' );
    if  ( pos1 == NULL ) 
        return;
    pos2 = strchr( pos1 + 1, '\"' );
    if  ( pos2 == NULL ) 
        return;
    pos3 = strchr( pos2 + 1, '>' );
    if  ( pos3 == NULL ) 
        return;
    pos2++;
    
    while  ( *pos3 ) {
        *pos2++ = *pos3++;
    }
    *pos2 = 0;

    strcpy( in_line, line );    
}


static void  handle_line_2( char  * in_line )
{
    char  line[ LINE_SIZE ], *pos, *pos1, *pos2;

    strcpy( line, in_line );
    pos = strstr( line, " ADD_DATE=" );
    if  ( pos == NULL ) 
        return;
    pos1 = strchr( pos, '\"' );
    if  ( pos1 == NULL ) 
        return;
    pos2 = strchr( pos1 + 1, '\"' );
    if  ( pos2 == NULL ) 
        return;
    pos2++;
    
    while  ( *pos2 ) {
        *pos++ = *pos2++;
    }
    *pos = 0;

    strcpy( in_line, line );    
}


static void  handle_line( char  * in_line )
{
    handle_line_1( in_line );
    handle_line_2( in_line );
    if  ( strstr( in_line, "IMG SRC" ) != NULL
          ||  strstr( in_line, "T COLOR" ) != NULL ) {
        *in_line = 0;
        return;
    }

    /*    if  ( strstr( in_line, "Sariel Har-Peled" ) != NULL ) {
        *in_line = 0;
        return;
        }*/
}

/*---------------------------------------------------------------------
 * Tree 
\*---------------------------------------------------------------------*/

bool   Tree::readLine( FILE  * fl, char  * line ) 
{
    char  * pos;

    while  ( !feof( fl ) ) {
        pos = fgets( line, LINE_SIZE, fl );
        if  ( pos == NULL )
            break;
        handle_line( line );
        if  ( *line == 0 ) 
            continue;
    
        if  ( ( strstr( line, "<DL>" ) == NULL ) 
              &&  ( strstr( line, "</DL>" ) == NULL )
              &&  ( strstr( line, "<DT>" ) == NULL )
              &&  ( strstr( line, "<DD>" ) == NULL ) ) 
            continue;
    
        str_replace( line, "<H3 FOLDED>", "" );
        str_replace( line, "<H3>", "" );
        str_replace( line, "</H3>", "" );
        str_replace( line, "<p>", "" );
        str_replace( line, "\n", "" );

        return  true;
    }

    return  false;
}

void   Tree::registerText( TreeNode  * curr, char  * line )
{
    str_replace( line, "<DD>", "" );  
    curr->registerDesc( line );
}

void   Tree::registerSon( TreeNode  * son, TreeNode  * father )
{
    father->registerSon( son );
    //arr[ father ].registerSon( son_pos );
}


TreeNode  * Tree::createEntry( TreeNode  * father, char  * line, int  depth )
{
    char  addr[ LINE_SIZE ], desc[ LINE_SIZE ], * pos, * end_pos;
    char  * start_addr, * start_title, * end_title;

    str_replace( line, "<DT>", "" );  
    addr[ 0 ] = desc[ 0 ] = 0;

    pos = strstr( line, "A HREF=" );
    if  ( pos == NULL ) { 
        TreeNode  *node = new  TreeNode( count_nodes++, line, NULL, father, depth );
    
        //arr.pushx( node );
        registerSon( node, father );

        return  node;
    }
  
    start_addr = pos + 8;
    end_pos = strstr( start_addr, "\"" );
    assert( end_pos != NULL );
    *end_pos = 0;

    strcpy( addr, start_addr );
    *end_pos = '\"';

    start_title = strstr( end_pos, ">" );
    assert( start_title != NULL );
    start_title++;

    end_title = strstr( start_title, "<" );
    *end_title = 0;

    strcpy( desc, start_title );
    *end_title = '<';

    TreeNode  * node = new TreeNode( count_nodes++, desc, addr, father, depth );
    //arr.pushx( node );
    registerSon( node, father );

    return  node;
}


void   Tree::construct_inner( FILE  * fl, TreeNode  * father, int  depth ) 
{
    TreeNode  * curr = father;
    char  line[ LINE_SIZE ];
    bool  fFirst;

    fFirst = true;
    while  ( readLine( fl, line ) ) {
        if  ( strstr( line, "<DD>" ) != NULL ) {
            registerText( curr, line );
            continue;
        }
        if  ( fFirst  &&  (depth == 0)  &&  ( strstr( line, "<DL>" ) != NULL ) ) {
            fFirst = false;
            continue;
        }
        if  ( strstr( line, "<DL>" ) != NULL ) {
            construct_inner( fl, curr, depth + 1 );
            continue;
        }
        if  ( strstr( line, "</DL>" ) != NULL ) 
            return;
        if  ( strstr( line, "<DT>" ) != NULL ) {
            curr = createEntry( father, line, depth );
            continue;
        }
        //printf( "????????????????????????? %s\n", line );
    }
}

void   Tree::dump()
{
    if  ( root != NULL )
        root->dumpRecursive();
    /*
      int  entry = 0;

      while  ( entry < arr.size() ) {
      if  ( ( arr[ entry ].getDepth() < 2 )  &&  ( arr[ entry ].isFolder() ) )
      arr[ entry ].dump();
      entry++;
      }*/
}


void   Tree::construct( char  * file_name ) 
{
    FILE  * fl;

    fl = fopen( file_name, "rt");
    if  ( fl == NULL ) {
        fprintf( stderr, "Unable to open file: [%s]\n", file_name );
        exit( -1 );
    }
  
    root = new   TreeNode( count_nodes++, 
                           "Bookmarks", "" );
    //arr.pushx( root );
  
    construct_inner( fl, root );
  
    fclose( fl );
}

void  Tree::computeHtmlAddr( TreeNode  * entry, char  * out )
{
    TreeNode  * curr;
  
    curr = entry;
    while  ( ( ! curr->isRoot() )  &&  ( ! curr->isCreateFile() ) ) {
        curr = curr->getFather();
    }
  
    //createF ileName( *cfg, curr->getID(), tmp, true );
    sprintf( out, "%s#lbl%05d", curr->getShortFileName(), entry->getID() );
}

int   Tree::computeRelDepth( TreeNode  * entry )
{
    int  count;

    count = 0;
    while  ( ! entry->isRoot() ) { 
        if  ( entry->isCreateFile() ) 
            break;

        entry = entry->getFather();
        count++;
    }

    return  count;
}

int  Tree::getTOCSize( TreeNode  * entry )
{
    int  count;
    TreeNode  * in_entry;

    count = 0;

    for  ( int  ind = 0; ind < entry->sonsNum(); ind++ ) {
        in_entry = entry->getSon( ind );
        if  ( ! in_entry->isFolder() )
            continue;
        if  ( in_entry->isCreateFile() ) 
            continue;
        count++;
    }

    return  count;
}


void  Tree::createTableOfContents( FILE  * fl, TreeNode  * entry )
{
    char  line[ LINE_SIZE ], line1[ LINE_SIZE ];
    TreeNode  * in_entry;

    if  ( getTOCSize( entry ) < 4 )
        return;

    fprintf( fl, "<HR><DL><DL>\n" );
    for  ( int  ind = 0; ind < entry->sonsNum(); ind++ ) {
        in_entry = entry->getSon( ind );
        if  ( ! in_entry->isFolder() ) {
            fprintf( stderr, "warning: Ignoring entry: [%s]\n", 
                     in_entry->getTitle() );
            continue;
        }
        in_entry->createHTML( line );
        computeHtmlAddr( in_entry, line1 );
        fprintf( fl, "<DT><a href=\"%s\">%s</a></DT>\n", line1, line );
    }
    fprintf( fl, "</DL></DL>\n" );
}


bool  Tree::printTitle( FILE  * fl, TreeNode  * node, char  * text, bool  fTop )
{
    char  line[ LINE_SIZE ];
    char  target[ LINE_SIZE ], tmp[ LINE_SIZE ];
    int  sz;
    TreeNode  * curr;

    sz = max( 1, 1 + computeRelDepth( node ) );
    if  ( node->isCreateFile()  &&  ( ! fTop ) ) {
        computeHtmlAddr( node, target );
        fprintf( fl, "<H%d><a href=\"%s\">%s</a></h%d>\n", sz, target, 
                 text, sz );
        return  true;
    } 
  
    if  ( ! fTop ) {
        fprintf( fl, "<H%d>%s</h%d>\n", sz, text, sz );
        return  false; 
    }

    strcpy( target, text );
    curr = node->getFather();
    while  ( curr != NULL ) {
        computeHtmlAddr( curr, line );
        sprintf( tmp, "<a href=\"%s\">%s</a> : %s", line, 
                 curr->getTitle(), target );
        strcpy( target, tmp );

        curr = curr->getFather();
    }

    fprintf( fl, "<h1>%s</h1>\n", target );

    return  false;
}

void  Tree::createFileInner( FILE  * fl, TreeNode  * node,
                             bool  fTop, bool  fMenu )
{
    char  line[ LINE_SIZE ];
    char  line1[ LINE_SIZE ];
    int  sz;
    bool  fAddedTable = false;

    node->createHTML( line );
    if  ( ! node->isFolder() ) {
        if  ( fMenu ) {
            return;
        }
        fprintf( fl, "<a name=\"lbl%05d\">\n", node->getID() );
        fprintf( fl, "<DT>%s</DT>\n", line );
        return;
    }
    if  ( fMenu  &&  ( node->getDepth() > 1 ) )
        return;
  
    fprintf( fl, "<a name=\"lbl%05d\">\n", node->getID() );
    sz = max( 1, 1 + node->getDepth() );
    if  ( fMenu ) {
        if  ( node->isRoot() ) {
            fprintf( fl, "<H1>Bookmarks</H1>\n" );
        } else {
            computeHtmlAddr( node, line1 );
            fprintf( fl, "<a href=\"%s\">%s</a>\n", line1, line );
        }
    } else {
        if  ( printTitle( fl, node, line, fTop ) )
            return;
    }

    if  ( fTop  &&  ( !fMenu ) ) 
        createTableOfContents( fl, node );
    if  ( fTop ) {
        fprintf( fl, "<HR>\n" );
        //fprintf( fl, "<DL>\n" );
    }
    fprintf( fl, "<DL>\n" );
    for  ( int  ind = 0; ind < node->sonsNum(); ind++ ) {
        if  ( node->isNeedSpace( ind )  &&  ( ! fMenu )  )
            fprintf( fl, "<p>\n" );

        if  ( node->isRoot()  &&  ( ! node->getSon( ind )->isFolder() ) ) {
            fAddedTable = true;
        }
        createFileInner( fl, node->getSon( ind ), false, fMenu );
    }
    //  if  ( fTop )
    //  fprintf( fl, "</DL>\n" );
    fprintf( fl, "</DL>\n" );

    // We need to generate entries for things appearing in the top
    // menu. We put it after the table of contents...
    if  ( fAddedTable ) {
        fprintf( fl, "<hr>\n" );
        for  ( int  ind = 0; ind < node->sonsNum(); ind++ ) {
            if  ( node->isNeedSpace( ind )  &&  ( ! fMenu )  )
                fprintf( fl, "<p>\n" );
            
            if  ( node->getSon( ind )->isFolder() ) 
                continue;
            
            createFileInner( fl, node->getSon( ind ), false, false );
        }
    }
}


void  Tree::createFile( TreeNode  * node, bool  fMenu )
{
    FILE  *fl;
    char  filename[ LINE_SIZE ];

    strcpy( filename, node->getFileName() );
    //creat eFileName( *cfg, node->getID(), tmp, false );
    //sprintf( filename, "%s", tmp );
    fl = fopen( filename, "wt" );
    if  ( fl == NULL ) {
        fprintf( stderr, "Unable to create file: [%s]\n", filename );
        exit( -1 );
    }

    printHTMLStart( *cfg, fl, node->getTitle() );

    createFileInner( fl, node, true, fMenu );

    time_t  tm;
    time( &tm );
    fprintf( fl, "<HR>\n" );
    fprintf( fl, "<!-- hhmts start -->\n" );
    if  ( cfg->f_give_credit ) 
        fprintf( fl, "Last <a href="
                 "\"http://www.math.tau.ac.il/~sariel/misc/bookmng.html\">modified</A>"
                 ": %s\n", ctime( &tm ) );
    else
        fprintf( fl, "Last modified: %s\n", ctime( &tm ) );
    fprintf( fl, "<!-- hhmts end -->\n" );
    fprintf( fl, "</body>\n" );
    fprintf( fl, "</html>\n" );
  
    fclose( fl );  
    if  ( cfg->f_verbose ) 
        fprintf( stderr, "File [%s] created\n", filename );

    count_files_created++;
}

void  Tree::createFilesInner( TreeNode  * node )
{
    if  ( ( !node->isRoot() )  &&  node->isCreateFile() )
        createFile( node, false );
  
    for  ( int  ind = 0; ind < node->sonsNum(); ind++ )
        createFilesInner( node->getSon( ind ) );
}

void  Tree::createFiles()
{
    createFilesInner( root );
}


void  Tree::createMenuFile()
{
    createFile( root, true );
}


int  Tree::computeFilesInner( TreeNode  * node )
{
    int  size = 0;

    assert( node != NULL );

    if  ( ! node->isFolder() )
        return  1;

    size = 1;
    for  ( int  ind = 0; ind < node->sonsNum(); ind++ ) {
        size += computeFilesInner( node->getSon( ind ) );
    }
    if  ( ( size > THRESHOLD_FILE )  ||  ( node->getDepth() < 1 ) ) {
        node->setCreateFile( true );
        return  1;
    }

    return  size;
}


void  Tree::computeFiles()
{
    computeFilesInner( root );
}


int  Tree::destroyByKillStrings()
{
    int  count;

    count = 0;
    for  ( int  ind = 0; ind < cfg->killStrings.size(); ind++ )
        count += root->destroyByString( cfg->killStrings[ ind ] );

    return  count;
}

int   Tree::getNumberFilesCreated() const
{
    return  count_files_created;
}

void  Tree::getMainFileName( char  * file )
{
    strcpy( file, root->getFileName() );
    //createFileName( *cfg, 0, file, false );
}


void   Tree::computeFileNamesInner( TreeNode  * node )
{
    if  ( node == NULL )
        return;
    
    createFileName( *cfg, node->getID(), node->getFileName(), false );
    createFileName( *cfg, node->getID(), node->getShortFileName(), true );

    for  ( int  ind = 0; ind < node->sonsNum(); ind++ ) 
        computeFileNamesInner( node->getSon( ind ) );
}


void   Tree::computeFileNames()
{
    computeFileNamesInner( root );
}

void  TreeNode::registerFileName( const  TreePath  & tp, int  entry,
                                  ConfigInfo  & cfg )
{
    TreeNode  * in_entry;

    if  ( ( entry >= tp.size() )  
          ||  ( stricmp( tp.line( entry ), line ) != 0 ) ) {
        printf( "Unable to register filename!" );
        tp.print();
        return;
    }

    if  ( entry == ( tp.size() - 1 ) ) {
        strcpy( filename_short, tp.filename() );
        strcpy( filename, cfg.dstFile );
        strcat( filename, filename_short );
        return;
    }
    
    for  ( int  ind = 0; ind < sonsNum(); ind++ ) {
        in_entry = getSon( ind );
        if  ( ! in_entry->isFolder() )
            continue;
        if  ( stricmp( tp.line( entry + 1 ), in_entry->line ) == 0 ) {
            in_entry->registerFileName( tp, entry + 1, cfg );
            return;
        }
    }
    printf( "Unable to register filename!" );
    tp.print();
}

void  Tree::registerAltFileNames()
{
    for  ( int  ind = 0; ind < cfg->altFilenamesArr.size(); ind++ ) 
        root->registerFileName( cfg->altFilenamesArr[ ind ], 0, *cfg );    
}


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 *     
 * Tree.C - End of File
 \*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
