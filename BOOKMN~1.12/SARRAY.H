/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 * sarray.h -
 *    Small/static templated array.
\*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#ifndef  __SARRAY__H
#define  __SARRAY__H


/*----------------------------------------------------------------------
 * a function we need to implement a search.
\*----------------------------------------------------------------------*/

int        searchArrayEntry( const void  * ptr, int  size, int num,
                                    const void  * var );


int        searchTableEntry( const void  * ptr,
                                    int           localOffset,
                                    int           localSize,
                                    int           size,
                                    int           numEnt,
                                    const void  * var );
int        calcEntriesNum( void    * ptr, int      size );


#ifndef  TRUE
#define  TRUE  1
#define  FALSE 0
typedef  int   BOOL;
#endif  /* TRUE */

template<class T>
void  * operator  new( size_t x, T  * ptr ) 
{
    (void)x;
    return  (void *)ptr;
}


/*template<class T>
void   operator  delete[]( void  * x, size_t  x, T  * ptr ) 
{
    (void)x;
    //turn  (void *)ptr;
}*/



template<class T>
class  Array {
private:
    BOOL        fInitialized;
    int         maxSize;
    int         currSize;
    T         * pArr;
    int  id;

    int  max( int  a, int  b )
      {
        return  ( a > b)? a : b;
      }

public:
    typedef  int        ( * ptrCompFunc )( const T     & a, const T    & b );
    typedef  int        ( * ptrCompFuncExt )( void  * data, const T     & a,
                                              const T    & b );
    void   dump() 
    {
        fflush( stdout );
        fflush( stderr );
        printf( "sarray dump\n" );
        printf( "   fInitialized: %d\n", fInitialized );
        printf( "   maxSize: %d\n", maxSize );
        printf( "   currSize: %d\n", currSize );
        printf( "   pArr: %p\n", (void *)pArr );
        printf( "   id: %d\n", id );
    }

private:
    /*    void        DASSERT( char   cond,
                        char  * str ) const
    {
        if  ( ! cond ) 
          fprintf( stderr, "SmallArray failure: [%s]\n", str ); 
    }*/

    int              Array<T>:: partitionExt( ptrCompFuncExt  pCompFunc,
                                              void  * data, long  left, long  right );
    int              Array<T>:: partition( ptrCompFunc  p_comp,
  //           int        ( * pCompFunc )( T     & a, T    & b ),
				       long                   left,
                                       long                   right );
    void             Array<T>::internalQSortExt( ptrCompFuncExt  pCompFunc,
                                                 void  * data, int  left, int  right );
    void             Array<T>::internalQSort( ptrCompFunc   pCompFunc,
                                              int           left, int        right );


    void   termRange( int  size ) 
    {
        int  ind;

        for  ( ind = 0; ind < size; ind++ )
            (pArr + ind)->~T();
        //delete( pArr )  (pArr + ind);
    }


public:
    Array( void )
    {
        fInitialized = FALSE;
    }

    void             Array<T>::quickSort( ptrCompFunc  pFunc );
    int              binarySearch( const T    & elem, ptrCompFunc  pCompFunc );
    int              binarySearchExt( const T    & elem, ptrCompFunc  pCompFunc,
                                      int  left, int  right );
    void             Array<T>::quickSortExt( ptrCompFuncExt  pCompFuncExt,
                                             void  * data );
    void             Array<T>::quickSortReverse( ptrCompFunc  pCompFunc );
    void             Array<T>::substractSet( Array     & set, ptrCompFunc  pCompFunc );
    void             Array<T>::substractSortedSet( Array      & set, ptrCompFunc  pCompFunc );

    void  permut()
    {
        int  newPos, ind;

        for   ( ind = 0; ind < currSize; ind++ ) {
          double  k;

            realRand(k);
            newPos = (to_int(k * (3 * currSize))) % currSize;
            assert( 0 <= newPos  &&  newPos < currSize );
            exchange( pArr[ ind ], pArr[ newPos ] );            
        }
    }


    int   hash( void ) const
    {
        int  ind, val, jnd;
        const char  * ptr;

        val = 0;
        for  ( ind = 0; ind < getEntriesNum(); ind++ ) {
            ptr = (char *)&((*this)[ ind ]);
            for  ( jnd = sizeof(T)-1; jnd >= 0; jnd-- ) {
                val = ((val ^ *ptr) << 3) + *ptr + val;
                ptr++;
            }
        }

        return  val;
    }


    void  truncate( int      pos )
    {
        DASSERT( pos >= 0, "truncate" );
        if  ( currSize >= pos  &&   pos >= 0 )
            currSize = pos;
    }
        
    void  reverse( void )
    {
        int                ind;
        T                  tmp;

/*        printf( "reversing\n" );*/
        for  ( ind = (currSize / 2) - 1; ind >= 0; ind-- ) { 
            tmp = (*this)[ ind ];
            (*this)[ ind ] = (*this)[ currSize - ind - 1 ];
            (*this)[ currSize - ind - 1 ] = tmp;
        }
    }


    BOOL  isFull( void ) const
    {
        return   currSize >= maxSize;
    }

    BOOL  isEmpty( void ) const
    {
        return   currSize <= 0;
    }

    void  initToEmpty( void )
    {
        fInitialized = FALSE;
    }

    void  init( int   entriesNum, int  _id )
    {
      int  tmp_size;

        id = _id;
        maxSize = entriesNum;
        currSize = 0;

        //pArr = new T[ max( sizeof( T ) * entriesNum, 256U ) ];
        //printf( "Malloc size: %d\n", (int)max( sizeof( T ) * entriesNum, 256U ) );
        
        tmp_size = sizeof( T ) * (2+ entriesNum);
        if  ( tmp_size < 256 )
          tmp_size = 256;
        pArr = (T *)malloc( ( tmp_size ) );
        //printf( "%p malloc: %d\n", pArr, id );
        if  ( pArr == NULL ) {
          /*  DEBUG_SystemPanic( __POS__, "Fail to allocate array" ); */
            return;
        }
        fInitialized = TRUE;
    }

    BOOL    resize( int        entriesNum )
    {
        int         size, ind;
        T         * pNewArr;
        
	size = max( sizeof( T ) * (2 + entriesNum), 256U );
        //pNewArr = new T[ size ];
        pNewArr = (T *)malloc( size );
        // printf( "%p malloc (resize): %d, size: %d\n", pNewArr, id, size );
        // fflush( stdout );

        if  ( pNewArr == NULL ) 
            return  FALSE;
	memset( pNewArr, 0, size );
        // memcpy( pNewArr, pArr, sizeof( T ) * currSize );

        for  ( ind = 0; ind < currSize; ind++ )
            ::new( pNewArr + ind )  T( pArr[ ind ] );

        //           pNewArr[ ind ] = pArr[ ind ];

        //delete[]  pArr;
        termRange( currSize );
        free( (void *) pArr );
        //printf( "%p free: %d\n", pArr, id );

        //printf( "malloc\n\n\n\n\n" );	
	pArr = pNewArr;
	maxSize = entriesNum;	

	return  TRUE;
    }
    
    BOOL       grow( int          size ) 
    {
        if  ( size <= maxSize )
            return  TRUE;
        
        return  resize( 256 + size * 2 );
    }

    const T &operator[]( int i )  const
    {
        DASSERT( 0 <= i  &&  i < currSize  &&  fInitialized, "range failure" );
        return( pArr[ i ] );
    }

    const T &getCellDirect( int i )  const
    {
        return( pArr[ i ] );
    }

    Array<T>::~Array( void )
    {
        term();
    }

    T &operator[]( int i )
    {
        DASSERT( 0 <= i  &&  i < currSize  &&  fInitialized, "range failure" );
        return( pArr[ i ] );
    }


int        Array<T>::getEntriesNum( void ) const
{
    return    currSize;
}
int        Array<T>::size( void ) const
{
    return    currSize;
}


void       Array<T>::deleteEntry( const T   & var )
{
    int                 ind;

    ind = searchEntry( var );
    if  ( ind >= 0 ) {
        pArr[ ind ] = pArr[ currSize - 1 ];
        currSize--;
    }
}


void       Array<T>::deleteEntryByPos( int       ind )
{
    DASSERT( 0 <= ind  &&  ind < currSize &&  fInitialized, "ranmge failure" );
    if  ( ind != (currSize - 1) ) {
        pArr[ ind ] = pArr[ currSize - 1 ];
    }
    currSize--;
}


void       Array<T>::moveToFront( int  pos )
{
    T  tmp;
    int                  ind;

    DASSERT( 0 <= pos  &&  pos < currSize &&  fInitialized, 
             "moveToFront: ranmge failure" );

    tmp = pArr[ pos ];
    for  (  ind = pos; ind > 0; ind-- ) 
        pArr[ ind ] = pArr[ ind - 1 ];

    pArr[ 0 ] = tmp;
}


void       Array<T>::deleteEntryByPosAndKeepOrder( int       pos )
{
    int                  ind;

    DASSERT( 0 <= pos  &&  pos < currSize &&  fInitialized, "ranmge failure" );

    for  (  ind = pos; ind < currSize - 1; ind++ ) 
        pArr[ ind ] = pArr[ ind + 1 ];

    currSize--;
}


void       Array<T>::term( void )
{
    if  ( ! fInitialized )
        return;
    if  ( pArr != NULL ) {
        //delete[]  pArr;
        termRange( currSize );
        free( (void *)pArr );
        //printf( "%p free\n", pArr );
        pArr = NULL;
    }

    maxSize = currSize = 0;
    fInitialized = FALSE;
}


void       Array<T>::simulateTerm( void )
{
    fInitialized = false;
    pArr = NULL;
}


void       Array<T>::insertEntry( const T   & var )
{
  assert( 0 <= currSize  &&  currSize < maxSize );
  memset( (void *)(pArr + currSize), 0, sizeof( T ) );
  ::new(pArr + currSize ) T( var );
  currSize++;
}


int        Array<T>::allocEntry()
{
    if  ( currSize >= maxSize ) {
        if  ( ! resize( 2 * currSize + 1 ) )
            return  -1;
    }
    
    DASSERT( currSize < maxSize, "insert failure" );
    memset( (void *)&(pArr[ currSize ]), 0, sizeof( T ) );
    currSize++;

    return  currSize - 1;
}


BOOL       Array<T>::growAndInsert( const T   & var )
{
    if  ( currSize >= maxSize ) {
        if  ( ! resize( 4 * currSize + 1 ) )
            return  FALSE;
    }
    insertEntry( var );

    return  TRUE;
}


void       Array<T>::pushx( const T   & var ) 
{
    assert( growAndInsert( var ) );
}


void       Array<T>::reset( void )
{
    currSize = 0;
}


int        Array<T>::searchEntry( const T   & var )
{
    return   searchArrayEntry( pArr, sizeof( T ), currSize, &var );
}

};



template<class T, int  maxSize>
class  SmallArray {
private:
    int         currSize;
    T           pArr[ maxSize ];

public:
    SmallArray( void )
    {
        currSize = 0;
    }

    BOOL  isFull( void ) const
    {
        return   currSize >= maxSize;
    }

    BOOL  isEmpty( void ) const
    {
        return   currSize <= 0;
    }

    void  init( void )
    {
        currSize = 0;
    }

    void  init( T        * ptr )
    {
        currSize = calcEntriesNum( (void *)ptr, sizeof( T ) );
        memcpy( pArr, ptr, currSize * sizeof( T ) );
    }

    void  flush( void )
    {
        currSize = 0;
    }

    const T    & lastEntry( void )  const
    {
        DASSERT( currSize > 0, "Small Array::no last element to look at" );
        return( pArr[ currSize - 1 ] );
    }

    T    & lastEntry( void )
    {
        DASSERT( currSize > 0, "Small Array::no last element to look at" );
        return( pArr[ currSize - 1 ] );
    }

    const T &operator[]( int i )  const
    {
        DASSERT( 0 <= i  &&  i < currSize,
                 "Small Array::Range failure" );
        return( pArr[ i ] );
    }

    const T &getEntry( int i )  const
    {
        DASSERT( 0 <= i  &&  i < currSize,
                 "Small Array::Range failure" );
        return( pArr[ i ] );
    }

    ~SmallArray( void )
    {
        term();
    }

    T &operator[]( int i )
    {
        DASSERT( 0 <= i  &&  i < currSize,
                 "Small Array::Range failure" );
        return( pArr[ i ] );
    }

    int        getEntriesNum( void ) const
    {
        return    currSize;
    }
    int        size( void ) const
    {
        return    currSize;
    }

    void       bringToTop( const T    & var )
    {
        int                 ind;

        ind = searchEntry( var );
        if  ( ind >= 0  &&  ind < currSize - 1 )
            exchange( pArr[ ind ], pArr[ currSize - 1 ] );
    }

    void       deleteEntryKeepOrder( const T      var )
    {
        int                   ind;

        ind = searchEntry( var );
        if  ( ind >= 0 ) {
            memcpy( (void *)&(pArr[ ind ]),
                     (void *)&(pArr[ ind + 1 ] ),
                     (currSize - ind - 1 ) * sizeof( T ) );
            currSize--;
        }
    }

    void       deleteEntry( const T   & var )
    {
        int                 ind;

        ind = searchEntry( var );
        if  ( ind >= 0 ) {
            pArr[ ind ] = pArr[ currSize - 1 ];
            currSize--;
        }
    }


    void       deleteEntryByPos( int       ind )
    {
        DASSERT( 0 <= ind  &&  ind < currSize,
                 "Small Array::delete entry Range failure" );
        pArr[ ind ] = pArr[ currSize - 1 ];
        currSize--;
    }

    void        pop( void )
    {
        DASSERT( currSize > 0, "Small Array::poping an empty stack" );
        if  ( currSize > 0 )
            currSize--;
    }

    void       term( void )
    {
        currSize = 0;
    }


    void       insertEntry( const T   & var )
    {
        DASSERT( currSize < maxSize, "Small Array::Insert failure" );
        pArr[ currSize ] = var;
        currSize++;
    }

    void       push( const T       & var )
    {
        insertEntry( var );
    }

    void       reset( void )
    {
        currSize = 0;
    }


    int        searchEntry( const T   & var )
    {
        return   searchArrayEntry( &pArr[0], sizeof( T ),
                                   currSize, &var );
    }

};


#endif  /* __SARRAY__H */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 *
 * sarray.h - End of File
\*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
