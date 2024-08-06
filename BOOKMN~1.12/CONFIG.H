/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 * config.h -
 *     
\*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#ifndef  __CONFIG__H
#define  __CONFIG__H

#ifndef  LINE_SIZE
#define  LINE_SIZE  1024
#endif  // LINE_SIZE 

class  ConfigInfo {
public:
    char  srcFile[ LINE_SIZE ], dstFile[ LINE_SIZE ], prefixFile[ LINE_SIZE ];
    bool  f_give_credit, f_verbose;
    Array<char *>  killStrings;
    Array<TreePath>  altFilenamesArr;
    
    
    ConfigInfo() {
        memset( this, 0, sizeof( ConfigInfo ) );
        killStrings.init( 10, 1 );
        altFilenamesArr.init( 10, 2 );
    }

    void  addKillString( char  * str ) {
        killStrings.pushx( strdup( str ) );
    }
    
    void  addAltFileName( TreePath  & path ) {
        altFilenamesArr.pushx( path );
    }    
};


#else   /* __CONFIG__H */
#error  Header file config.h included twice
#endif  /* __CONFIG__H */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 *     
 * config.h - End of File
\*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/


