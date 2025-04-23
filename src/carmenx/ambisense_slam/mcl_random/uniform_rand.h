#ifndef AMBISENSE_LOCALIZE__UNIFORM_RAND_H
#define AMBISENSE_LOCALIZE__UNIFORM_RAND_H

#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <boost/random.hpp>


/** Uniform pseudo-random number generator. Accords to polar form of Box-Muller transformation.
  */
// --------------------------------------------------------------------------
template< class FLOAT_TYPE=double >
class UniformRand
// --------------------------------------------------------------------------
{
public:
  UniformRand();
  ~UniformRand();

  /** Get a single random number from a uniform distribution over the interval [0,1]. */
  FLOAT_TYPE getRandomNumber();

  /** Get a single random number from a uniform distribution over the interval [a,a+intervalLength]. */
  FLOAT_TYPE getRandomNumber( FLOAT_TYPE a, FLOAT_TYPE intervalLength );

  /** Get a single random integer from a uniform distribution over the interval [a,a+intervalLength]. */
  int getRandomInteger( int a, int intervalLength );

protected:
  FLOAT_TYPE m_min, m_max, m_diff;
  uint m_int_diff;

  /// 2.0f / m_diff
  FLOAT_TYPE m_1_diff;

  typedef boost::lagged_fibonacci607 MyRandomGeneratorType;  // fastest!!
//  typedef boost::mt19937 MyRandomGeneratorType;

  // Boost random number generator
  MyRandomGeneratorType * m_pGenerator;
};


#include "uniform_rand.cpp"


#endif
