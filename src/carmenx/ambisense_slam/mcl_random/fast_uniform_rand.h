#ifndef AMBISENSE_LOCALIZE__FAST_UNIFORM_RAND_H
#define AMBISENSE_LOCALIZE__FAST_UNIFORM_RAND_H

#include "uniform_rand.h"


/** Fast uniform pseudo-random number generator. Bases on UniformRand, but
 *  precomputes uniform random values
  * and stores them in an array.
  * @see CUniformRand
  */
// --------------------------------------------------------------------------
template< class FLOAT_TYPE=double >
class FastUniformRand
// --------------------------------------------------------------------------
{
public:
  /** Create a uniform random number generator that precomputes and stores
   *  numberOfRandomValues uniformly distributed random values as floating
   *  point numbers.
   *   @param  numberOfRandomValues  The number of random values to be precomputed.
   */
  FastUniformRand( unsigned long numberOfRandomValues );
  ~FastUniformRand();

  /** Get a single random number from a uniform distribution over the interval [0,1]. */
  inline FLOAT_TYPE getRandomNumber()
    {
      moveToNextRandomValue();
      return *m_currentRandomValue;
    }

  /** Get a single random number from a uniform distribution over the interval [a,a+intervalLength]. */
    inline FLOAT_TYPE getRandomNumber( FLOAT_TYPE a, FLOAT_TYPE intervalLength )
    {
      return getRandomNumber() * intervalLength + a;
    }

  /** Get a single random integer from a uniform distribution over the interval [a,a+intervalLength].
    *  @deprecated  Use your own conversion method for integer values in order to control that process better.
    */
  inline int getRandomInteger( int a, int intervalLength )
    {
      return (int) getRandomNumber( a, a+intervalLength );
    }

  inline void moveToNextRandomValue();


protected:
  /// Number of random values which are provided
  unsigned long m_numberOfRandomValues;

  /// Array of precomputed random numbers
  FLOAT_TYPE *m_randomValues;

  /// Pointer to current precomputed random number
  FLOAT_TYPE *m_currentRandomValue;

  /// The underlying uniform random number generator
  UniformRand<FLOAT_TYPE> *m_pUniform;

  /// The index of the last returned random value
  int m_currentRandomValueIndex;

};

#include "fast_uniform_rand.cpp"


#endif
