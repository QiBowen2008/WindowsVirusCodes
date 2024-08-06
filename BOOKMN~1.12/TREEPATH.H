/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 * treepath.h -
 *     Stores an explicit path in the bookmarks tree.
\*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#ifndef  __TREEPATH__H
#define  __TREEPATH__H

typedef char   line_type[ LINE_SIZE ];

#define  MAX_PATH_DEPTH   10

class  TreePath {
private:
    SmallArray<line_type, MAX_PATH_DEPTH>   lines;
    line_type  new_file_name;
public:
    TreePath() : lines() {};

    ~TreePath() {
        lines.term();
    }

    int  size() const {
        return  lines.size();
    }

    const char  * line( int  ind )  const {
        return  lines[ ind ];
    }

    void  push( const char  * str ) {
        line_type  l;

        assert( strlen( str ) < LINE_SIZE );
        strcpy( l, str );
        lines.push( l );
    }

    void  init( char  * line, FILE  * fl );

    void  print()  const {
        printf( "<<<\n" );
        for  ( int  ind = 0; ind < lines.size(); ind++ ) 
            printf( "  %s\n", lines[ ind ] );
        printf( "Filename: [%s]\n", new_file_name );
        printf( ">>>\n" );
    }

    const char   * filename() const {
        return new_file_name;
    }

};


#else   /* __TREEPATH__H */
#error  Header file treepath.h included twice
#endif  /* __TREEPATH__H */

/* treepath.h - End of File ------------------------------------------*/


