#ifndef CROCO__UTILS__FACTORIAL_H
#define CROCO__UTILS__FACTORIAL_H


/** Compute n*(n-1)*...*2*1 = n! */
// --------------------------------------------------------------------------
inline long unsigned int factorial( const int n )
// --------------------------------------------------------------------------
{
  if ( n < 2 )
    return 1;
  else
    return n * factorial( n - 1 );
}


/** Compute n*(n-1)*...(k+1) = n!/k! */
// --------------------------------------------------------------------------
inline long unsigned int partial_factorial( const int n, const int k )
// --------------------------------------------------------------------------
{
  if ( n == k )
    return 1;
  else
    return n * partial_factorial( n - 1, k );
}


#endif // CROCO__UTILS__FACTORIAL_H
