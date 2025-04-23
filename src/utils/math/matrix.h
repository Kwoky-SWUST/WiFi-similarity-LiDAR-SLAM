#ifndef ALLEMANIACDS3D__UTILS__MATRIX_H
#define ALLEMANIACDS3D__UTILS__MATRIX_H

#include <iostream>
#include <utils/debug/assert.h>


/** Matrix class
 *
 *  @author Written by Andreas Strack (strack@i5.informatik.rwth-aachen.de).
 *          Slightly modified by Philipp Vorst (some speed-ups)
 *
 */
// --------------------------------------------------------------------------
template< class SCALAR >
class Matrix
// --------------------------------------------------------------------------
{
  public:
    typedef Matrix<SCALAR> MATRIX;

    /** (Standard) constructor */
    Matrix()
    {
      ROWS = 0;
      COLS = 0;
      values = NULL;
    }

    /** Constructor for a matrix with m rows and n columns. */
    Matrix( unsigned int m,
            unsigned int n )
      {
        ROWS = m;
        COLS = n;
        values = (float*) malloc( ROWS*COLS*sizeof(SCALAR) );
      }

    /** Constructor for a matrix with m rows and n columns. All entries will be
     *  initialized with the value of init. */
    Matrix( unsigned int m,
            unsigned int n,
            SCALAR       init )
    {
      ROWS = m;
      COLS = n;

      InitMatrix( init );
    }

    /**
     * Copy constructor
     *
     */
    Matrix( const MATRIX & matrix )
    {
      // Setting ROWS = COLS = 0 is essential to indicate that
      // the array 'values' has not been created, yet!
      ROWS = 0;
      COLS = 0;
      ( *this ) = matrix;
    }

    /**
     * Destructor
     */
    ~Matrix()
    {
      free( values );
    }

    /**
     * Getter routines
     *
     */
    const unsigned int & m() const
    {
      return ROWS;
    }

    const unsigned int & n() const
    {
      return COLS;
    }


    /**
     * Setter routines
     *
     */
    void setDimensions( unsigned int m = 1,
                        unsigned int n = 1,
                        SCALAR init = 0 )
      {
        ROWS = m;
        COLS = n;
        InitMatrix( init );
      }


    /**
     * return the identity matrix
     *
     */
    static const MATRIX GetIdentityMatrix( const unsigned int dim )
    {
      MATRIX result( dim, dim, 0 );

      unsigned int index = 0;
      unsigned int step = dim + 1;
      for( unsigned int i = 0; i < dim; ++i )
        {
          result.values[ index ] = 1;
          index += step;
        }

      return result;
    }

    /**
       * return the identity matrix
       *
     */
    static const MATRIX GetDiagonalMatrix( const unsigned int dim, const SCALAR diag )
    {
      MATRIX result( dim, dim, 0 );

      unsigned int index = 0;
      unsigned int step = dim + 1;
      for( unsigned int i = 0; i < dim; ++i )
      {
        result.values[ index ] = diag;
        index += step;
      }

      return result;
    }

    /**
     * return the null matrix
     *
     */
    static const MATRIX GetNullMatrix( const unsigned int dim )
      {
        MATRIX result( dim, dim, 0 );
        return result;
      }


    /**
     * Tests whether the matrix is the null matrix
     *
     *
     * @return true if all entries of the matrix are null,
     *         false otherwise
     */
    bool IsNull() const
      {
        for( unsigned int m = 0; m < ROWS; ++m )
          {
            for( unsigned int n = 0; n < COLS; ++n )
              {
                if( values[ m*COLS+n ] != 0 )
                    return false;
              }
          }

        return true;
      }


    /**
     * calculate the reduced matrix resulting from deleting
     * one row and one column
     */
    MATRIX GetReduced( const unsigned int row,
                      const unsigned int col ) const
      {
        MATRIX result( ROWS - 1, COLS - 1 );

        for( unsigned int m = 0; m < ROWS; m++ )
          {
            for( unsigned int n = 0; n < COLS; n++ )
              {
                if(( m < row ) && ( n < col ))
                  {
                    result[m][n] = values[m*COLS+n];
                  }
                else if(( m > row ) && ( n < col ))
                  {
                    result[m - 1][n] = values[m*COLS+n];
                  }
                else if(( m > row ) && ( n > col ))
                  {
                    result[m - 1][n - 1] = values[m*COLS+n];
                  }
                else if(( m < row ) && ( n > col ))
                  {
                    result[m][n - 1] = values[m*COLS+n];
                  }
              }
          }

        return result;
      }


    /**
     * transpose
     *
     */
    const MATRIX GetTranspose() const
    {
      MATRIX result( COLS, ROWS );
      transposeTo( result );
      return result;
    }

    inline void transposeTo( MATRIX & result ) const
    {
      for( unsigned int m = 0; m < ROWS; ++m )
      {
        for( unsigned int n = 0; n < COLS; ++n )
        {
          result[n][m] = values[m*COLS+n];
        }
      }
    }


    inline MATRIX & Transpose()
    {
      if ( ROWS == COLS )
      {
        SCALAR copy;
        for( unsigned int m = 0; m < ROWS; ++m )
        {
          for( unsigned int n = m; n < COLS; ++n )
          {
            copy = values[m*COLS+n];
            values[m*COLS+n] = values[n*COLS+m];
            values[n*COLS+m] = copy;
          }
        }
      }
      else
      {
        ( *this ) = GetTranspose();
      }

      return ( *this );
    }


    /**
     * calculate the reduced matrix resulting from deleting
     * one row and one column
     */
    inline const MATRIX GetReduced( MATRIX & matrix,
                            unsigned int row,
                            unsigned int col )
      {
        MATRIX result( ROWS - 1, COLS - 1 );

        for( unsigned int m = 0; m < ROWS; ++m )
          {
            for( unsigned int n = 0; n < COLS; ++n )
              {
                if(( m < row ) && ( n < col ))
                  {
                    result[m][n] = values[m*COLS+n];
                  }
                else if(( m > row ) && ( n < col ))
                  {
                    result[m - 1][n] = values[m*COLS+n];
                  }
                else if(( m > row ) && ( n > col ))
                  {
                    result[m - 1][n - 1] = values[m*COLS+n];
                  }
                else if(( m < row ) && ( n > col ))
                  {
                    result[m][n - 1] = values[m*COLS+n];
                  }
              }
          }

        return result;
      }


    /**
     * calculate the determinant of the matrix
     * calls the recursive function determinant_iter
     *
     */
    inline const SCALAR GetDeterminant() const
      {
        return Determinant_Iter(( *this ));
      }

    /**
     * invert
     * used Lutz's code here
     * don't know where the hell he got it from,
     * but it works ;-)
     */
    inline const MATRIX GetInvert() const
    {
      MATRIX result( *this );
      invertTo( result );
      return result;
    }


    /**
     * invert
     * used Lutz's code here
     * don't know where the hell he got it from,
     * but it works ;-)
     */
    inline void invertTo( MATRIX & result ) const
    {
      // Special case for 2x2 Matrix
      if ( (ROWS == 2)  && (COLS == 2) )
      {
        // Then: Matrix = (a b)
        //                (c d)
        SCALAR & a = values[0];
        SCALAR & b = values[1];
        SCALAR & c = values[2];
        SCALAR & d = values[3];

        SCALAR factor = 1 / ( a*d - b*c );

        result[0][0] = factor *  d;
        result[0][1] = factor * -b;
        result[1][0] = factor * -c;
        result[1][1] = factor *  a;
      }
      else // no 2x2 matrix
      {

        SCALAR big, dum, pivinv;
        unsigned int icol = 0;
        unsigned int irow = 0;
  //         std::vector<unsigned int> indxc, indxr, ipiv;
  //
  //         indxc.resize( COLS, 0 );
  //         indxr.resize( COLS, 0 );
  //         ipiv.resize ( COLS, 0 );
        unsigned int indxc[COLS], indxr[COLS], ipiv[COLS];
        for ( unsigned int i = 0; i < COLS; ++ i )
        {
          indxc[i] = indxr[i] = ipiv[i] = 0;
        }
  //         unsigned int* indxc = (unsigned int*) calloc( COLS, sizeof(unsigned int) );
  //         unsigned int* indxr = (unsigned int*) calloc( COLS, sizeof(unsigned int) );
  //         unsigned int* ipiv  = (unsigned int*) calloc( COLS, sizeof(unsigned int) );

          unsigned int i, j, k, l, ll;


          /**
           * no error checking here ;-)
           *
           */
  /*       if (m!=n) */
  /* 	{ */
          /* 	  std::cerr << "Matrix nicht invertierbar !!! Rueckgabe der urspruenglichen Matrix!" << std::endl; */
  /* 	  return result; */
  /* 	} */

          for ( i = 0; i < COLS; ++i )
            {
              big = 0.0;

              for ( j = 0; j < COLS; ++j)
                {
                  if ( ipiv[j] != 1 )
                    {
                      for (k = 0; k < COLS; ++k)
                        {
                          if ( ipiv[k] == 0 )
                            {
                              if (sabs(result[j][k]) >= big)
                                {
                                  big = sabs( result[j][k] );
                                  irow = j;
                                  icol = k;
                                }
                            }
                        }
                    }
                }

              ++ipiv[icol];

              if( irow != icol )
                {
                  for ( l = 0; l < COLS; ++l )
                    {
                      dum = result[irow][l];
                      result[irow][l] = result[icol][l];
                      result[icol][l] = dum;
                    }
                }

              indxr[i] = irow;
              indxc[i] = icol;

              pivinv = 1 / result[icol][icol];

              result[icol][icol] = 1;

              for ( l = 0; l < COLS; ++l)
              {
                result[icol][l] *= pivinv;
              }

              for ( ll = 0; ll < COLS; ++ll )
                {
                  if( ll != icol )
                    {
                      dum = result[ll][icol];
                      result[ll][icol] = 0;
                      for ( l = 0; l < COLS; ++l )
                        {
                          result[ll][l] -= result[icol][l] * dum;
                        }
                    }
                }
            } // for i

          for (l = COLS - 1; l < COLS; --l )
          {
            if( indxr[l] != indxc[l] )
              {
                for ( k = 0; k < COLS; ++k )
                  {
                    dum = result[k][indxr[l]];
                    result[k][indxr[l]] = result[k][indxc[l]];
                    result[k][indxc[l]] = dum;
                  }

              }
          }
        }
      }


    inline MATRIX & Invert()
    {
      ( *this ) = GetInvert();

      return ( *this );
    }

    /**
     * OPERATORS
     *
     */

    /**
     * [] operator
     */
    inline SCALAR * operator [] ( const unsigned int row ) const
    {
      return (SCALAR*) &values[ row*COLS ];
    }


    /**
     * = operators
     *
     */
    MATRIX & operator = ( const MATRIX & matrix )
    {
/*      std::cout << "Creating new copy #1" << std::endl;*/
      if ( (ROWS != matrix.ROWS) || (COLS != matrix.COLS) || (values == NULL) )
      {
        // If 'values' array existed before, we have to re-allocate instead
        // of newly create memory space
        if ( ROWS > 0 || COLS > 0 )
          values = (float*) realloc( values, matrix.ROWS * matrix.COLS * sizeof(SCALAR) );
        else
          values = (float*) malloc( matrix.ROWS * matrix.COLS * sizeof( SCALAR ) );

        ROWS = matrix.ROWS;
        COLS = matrix.COLS;
      }
      memcpy( values, matrix.values, ROWS * COLS * sizeof(SCALAR) );

      return ( *this );
    }


    /**
     * '* SCALAR' operator
     *
     */
    const MATRIX operator *( const SCALAR & scalar ) const
      {
        MATRIX result( ROWS, COLS );

        for( unsigned int m = 0; m < ROWS; ++m )
          {
            for( unsigned int n = 0; n < COLS; ++n )
              {
                result[m][n] = values[m*COLS+n] * scalar;
              }
          }

        return result;
      }

    MATRIX & operator *=( const SCALAR & scalar )
    {
      for( unsigned int m = 0; m < ROWS; ++m )
      {
        for( unsigned int n = 0; n < COLS; ++n )
        {
          values[m*COLS+n] *= scalar;
        }
      }

      return ( *this );
    }

    /**
     * *Matrix operator
     *
     */
    const MATRIX operator *( const MATRIX & matrix ) const
      {
        MATRIX result( ROWS, matrix.n() );

        multTo( matrix, result );
//         for( unsigned int i = 0; i < ROWS; i++ )
//           {
//             for( unsigned int j = 0; j < matrix.n(); j++ )
//               {
//                 /**
//                  * calculate result[i][j] =
//                  * this[i][1]*matrix[1][j] + ... + this[i][COLS]*matrix[COLS][j]
//                  */
//                 for( unsigned int n = 0; n < COLS;  n++ )
//                   {
//                     result[i][j] += values[i*COLS+n] * matrix[n][j];
//                   }
//               }
//           }

        return result;
      }


    /** Multiplication of this and the given matrix, result is written to
     *  'result'.
     */
    inline void multTo( const MATRIX & matrix, MATRIX & result ) const
    {
      ASSERT( result.ROWS == ROWS,        "[Matrix::mult]  Invalid number of rows of result matrix. Should be " << ROWS );
      ASSERT( result.COLS == matrix.COLS, "[Matrix::mult]  Invalid number of columns of result matrix. Should be " << matrix.COLS );

      // Result gets confused if it is written to a multiplication source
      // matrix!
      ASSERT( &matrix != &result, "[Matrix::multTo]  Matrix to multiply and result are equal objects" );

      for ( unsigned int i = 0; i < ROWS; ++i )
      {
        for ( unsigned int j = 0; j < matrix.n(); ++j )
        {
          result[i][j] = 0;
          for( unsigned int n = 0; n < COLS;  n++ )
          {
            result[i][j] += values[i*COLS+n] * matrix[n][j];
          }
        }
      }
    }

    MATRIX & operator *=( const MATRIX & matrix )
    {
      ( *this ) = ( *this ) * matrix;

      return ( *this );
    }

    /**
     * + operator
     *
     */
    const MATRIX operator + ( const MATRIX & matrix ) const
    {
      MATRIX result( ROWS, COLS );
      addTo( matrix, result );
      return result;
    }


    void addTo( const MATRIX & matrix, MATRIX & result ) const
    {
      for( unsigned int m = 0; m < ROWS; m++ )
        {
          for( unsigned int n = 0; n < COLS; n++ )
            {
              result[m][n] = values[m*COLS+n] + matrix[m][n];
            }
        }
    }


    MATRIX & operator += ( const MATRIX & matrix )
    {
//      ( *this ) = ( *this ) + matrix;

      for( unsigned int m = 0; m < ROWS; m++ )
      {
        for( unsigned int n = 0; n < COLS; n++ )
        {
          values[m*COLS+n] += matrix[m][n];
        }
      }

      return ( *this );
    }

    /**
     * + operator
     *
     */
    const MATRIX operator -( const MATRIX & matrix ) const
    {
      MATRIX result( ROWS, COLS );
      subtractTo( matrix, result );
      return result;
    }


    /**
     * + operator
     *
     */
    void subtractTo( const MATRIX & matrix, MATRIX & result ) const
    {
      for( unsigned int m = 0; m < ROWS; m++ )
      {
        for( unsigned int n = 0; n < COLS; n++ )
        {
          /*                std::cout << "[" << m << ", " << n << "]: " << values[m*COLS+n] << " - " << matrix[m][n] << std::endl;*/
          result[m][n] = values[m*COLS+n] - matrix[m][n];
        }
      }
    }

    MATRIX & operator -=( const MATRIX & matrix )
      {
//       ( *this ) = ( *this ) - matrix;

        for( unsigned int m = 0; m < ROWS; m++ )
        {
          for( unsigned int n = 0; n < COLS; n++ )
          {
            values[m*COLS+n] -= matrix[m][n];
          }
        }

        return ( *this );
      }



    /**
     * << operator
     *
     */
    inline friend std::ostream & operator<<( std::ostream &ostr, const MATRIX & matrix )
    {
      for( unsigned int m = 0; m < matrix.m(); m++ )
      {
        for( unsigned int n = 0; n < matrix.n(); n++ )
        {
          ostr << matrix[m][n] << ( n == matrix.n() - 1 ? "" : " " );
        }

        ostr << ( (m == matrix.m()-1) ? "" : "\n" );
      }

      return ostr;
    }

  protected:
    /**
     * PRIVATE MEMBER FUNCTIONS
     *
     */

    /**
     * Initialize the matrix
     *
     * @param init the SCALAR to fill the matrix with
     */
    inline void InitMatrix( const SCALAR init = 0 )
    {
      if ( init == 0 )
      {
        values = (float*) calloc( ROWS*COLS, sizeof(SCALAR) );
      }
      else
      {
        values = (float*) malloc( ROWS*COLS*sizeof(SCALAR) );
        float* array_pointer = values;
        for ( unsigned int i = 0; i < ROWS*COLS; ++i )
        {
          *array_pointer = init;
          ++array_pointer;
        }
      }
    }

    /**
     * calculates absolute value of a SCALAR
     */
    inline const SCALAR sabs( const SCALAR scalar ) const
    {
      return ( scalar > 0 ? scalar : -scalar );
    }


    /**
     * calculate the determinant of a matrix recursively
     * assume the matrix is a square one
     *
     */
    const SCALAR Determinant_Iter( const MATRIX & matrix ) const
      {
        if ( matrix.m() == 1 )
          {
            return matrix[0][0];
          }

        SCALAR factor = 1;
        SCALAR sum = 0;

        /**
         * always calculate using the first row
         */
        for ( unsigned int n = 0; n < matrix.n(); ++n )
          {
            sum += factor * matrix[0][n] * Determinant_Iter( matrix.GetReduced( 0, n ));
            factor *= -1;
          }

        return sum;
      }



    /**
     * PRIVATE MEMBER VARIABLES
     *
     */

    /**
     * the size of the matrix
     */
    unsigned int ROWS;
    unsigned int COLS;

    /**
     * the actual matrix is stored in a
     + two-dimensional array
     */
    SCALAR* values;
};

#endif // ALLEMANIACDS3D__UTILS__MATRIX_H
