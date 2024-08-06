/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 * Tree.h -
 *     
\*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#ifndef  __TREE__H
#define  __TREE__H

#define  LINE_SIZE       1024
#define  MAX_SONS        100
#define  MAX_LINES       10
#define  THRESHOLD_FILE  1

class ConfigInfo;
class TreePath;

typedef  struct  {
    char   str[ LINE_SIZE ];
}  line_t;
typedef  SmallArray<line_t, MAX_LINES>   stack_lines_t;

class  TreeNode { 
private:
    char  line[ LINE_SIZE ];
    char  addr[ LINE_SIZE ];
    char  desc[ LINE_SIZE ];
    char  filename[ LINE_SIZE ];
    char  filename_short[ LINE_SIZE ];
    int  depth, id;
    bool  fCreateFile;
    SmallArray<TreeNode *, MAX_SONS>  sons;
    TreeNode  * father;

public:
    TreeNode( int  _id, char  * _line, char  * _link, TreeNode  * _father = NULL,
              int  _depth = 0 ) {
        memset( this, 0, sizeof( TreeNode ) );
        sons.init();
        father = _father;
        depth = _depth;
        id = _id;
        
        if  ( _line != NULL ) {
            while  ( ( *_line == ' ' )  ||  ( *_line == '\t' ) )
                _line++;
            strcpy( line, _line );
        }
        if  ( _link != NULL ) 
            strcpy( addr, _link );    
    }
    
    ~TreeNode() {
        for  ( int  ind = 0; ind < sons.size(); ind++ ) {
            delete  sons[ ind ];
            sons[ ind ] = NULL;
        }
    }

    int   size() {
        int  count;
    
        count = 1;
        for  ( int  ind = 0; ind < sons.size(); ind++ ) 
            count += sons[ ind ]->size();

        return  count;
    }

    // return the number of lines destroyed
    int   destroyByString( char  * str ) {
        int  count;

        count = 0;
        for  ( int  ind = 0; ind < sons.size(); ind++ ) 
            if  ( strcasecmp( sons[ ind ]->line, str ) == 0 ) {
                printf( "Deleting: [%s]\n", sons[ ind ]->line );
                count += sons[ ind ]->size();
                delete sons[ ind ];
                sons.deleteEntryByPos( ind );
            }  else
                count += sons[ ind ]->destroyByString( str );

        return  count;
    }

    int  getID()  const {
        return  id;
    }
    void  setCreateFile( bool  flag ) {
        fCreateFile = flag;
    }
    bool  isCreateFile() const {
        return  fCreateFile;
    }
    TreeNode  * getFather() const {
        return  father;
    }
    const char  * getTitle() const {
        return  line;
    }

    int  sonsNum()  const {
        return  sons.size();
    }
    TreeNode  * getSon( int  pos )  const {
        return  sons[ pos ];
    }
    bool  isRoot()  const {
        return  father == NULL;
    }
    void  createHTML( char  * out )
        {
            *out = 0;
            if  ( ( *line != 0 )  &&  ( *addr != 0 ) ) {
                sprintf( out, "<A HREF =\"%s\">%s</A>", addr, line );
            } else
                if  ( *line != 0 )
                    strcpy( out, line );
            if  ( *desc != 0 ) {
                strcat( out, "<BR><DL>" );
                strcat( out, desc );
                strcat( out, "</DL>" );
            }
        }

    bool  isFolder() const {
        return  ( sons.size() > 0 );
    }
    int  getDepth() const {
        return  depth;
    }
    void  registerDesc( char  * _text )
        {
            strcat( desc, _text );
        }
    void   registerSon( TreeNode  * son )
        {
            sons.push( son );
        }

    void  dump() {
        char  sline[ 100 ];
    
        sline[ 0 ] = 0;
        for  ( int  ind = 0; ind < depth; ind++ ) 
            strcat( sline, "      " );
    
        printf( "%3d:%s%s\n", depth, sline, line );
        /*    if  ( *addr != 0 )
              printf( "\t%s[%s]\n", sline, addr );
              if  ( *text != 0 )
              printf( "\t%s[%s]\n", sline, text );
        */
    }

    void  dumpRecursive() {
        dump();
        for  ( int  ind = 0; ind < sons.size(); ind++ ) 
            sons[ ind ]->dumpRecursive();
    }

    bool  isNeedSpace( int  ind ) {
        if  ( ind <= 0 ) 
            return  false;
        TreeNode  & nodea( *getSon( ind - 1 ) );
        TreeNode  & nodeb( *getSon( ind ) );
        
        return  ( nodea.isFolder()  &&  (! nodeb.isFolder() ) );
    }

    const char  * getFileName() const {
        return  filename;
    }
    const char  * getShortFileName() const {
        return  filename_short;
    }
    char  * getFileName() {
        return  filename;
    }
    char  * getShortFileName() {
        return  filename_short;
    }
    void  registerFileName( const TreePath  & tp, int  entry,
                            ConfigInfo  & cfg );
};

class  Tree {
private:
    TreeNode  * root;
    ConfigInfo  * cfg;
    int  count_nodes, count_files_created;

    void   construct_inner( FILE  * fl, TreeNode  * father, int  depth = 0 ) ;
    void   registerText( TreeNode  * curr, char  * line );
    void   createFileInner( FILE  * fl, TreeNode  * entry, bool  fTop, bool  f_menu );
    void   createFile( TreeNode  * entry, bool  f_menu );
    int    computeFilesInner( TreeNode  * entry );

public:
    Tree( ConfigInfo  & _cfg ) {
        memset( this, 0, sizeof( Tree ) );
        root = NULL;
        cfg = &_cfg;
        count_nodes = 0;
        //arr.init( 100, 100 );
    }
    ~Tree() { 
        //    arr.term();
    }
    bool   readLine( FILE  * fl, char  * line );
    void   construct( char  * file ); 
    TreeNode  * createEntry( TreeNode  * father, char  * line, int  depth );
    void   dump();
    void   registerSon( TreeNode  * son, TreeNode  * father );
    void   createFilesInner( TreeNode  * node );
    void   createFiles();
    void   computeFiles();
    void   computeHtmlAddr( TreeNode  * entry, char  * out );
    void   createMenuFile();
    int    computeRelDepth( TreeNode  * entry );
    int   getTOCSize( TreeNode  * entry );
    void  createTableOfContents( FILE  * fl, TreeNode  * entry );
    //bool  isNeedSpace( TreeNode  & node, int  ind );
    bool  printTitle( FILE  * fl, TreeNode  * entry, char  * text, bool  fTop );

    int   destroyByKillStrings();

    int   getNumberFilesCreated() const;
    void  getMainFileName( char  * file );
  
    void  computeFileNames();
    void  computeFileNamesInner( TreeNode  * node );

    void  registerAltFileNames();
};


#else   /* __TREE__H */
#error  Header file Tree.h included twice
#endif  /* __TREE__H */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 *     
 * Tree.h - End of File
 \*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
