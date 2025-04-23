#ifndef AMBISENSE_LOCALIZE__FAST_GAUSS_RAND_H
#define AMBISENSE_LOCALIZE__FAST_GAUSS_RAND_H

#include "gauss_rand.h"


/** Gaussian pseudo-random number generator. Bases on class GaussRand, but
 *  precomputes Gaussian numbers in order to provide them quickly.
 *  @see GaussRand
 *  @author Philipp Vorst
 */
// --------------------------------------------------------------------------
template< class FLOAT_TYPE=double >
class FastGaussRand
// --------------------------------------------------------------------------
{
public:
  FastGaussRand( FLOAT_TYPE mean, FLOAT_TYPE sigma, uint numberOfRandomValues );
  virtual ~FastGaussRand();

  FLOAT_TYPE getMean() { return m_pGauss->getMean(); }

  FLOAT_TYPE getSigma() { return m_pGauss->getSigma(); }

  /** Get a single random number from a gaussian distribution. */
  inline FLOAT_TYPE getRandomNumber()
    {
      moveToNextRandomValue();
      return m_randomValues[ m_currentRandomValueIndex ];
    }

  /** Get a pair of random numbers from a gaussian distribution. */
  inline GaussNumberPair<FLOAT_TYPE> getRandomNumberPair();

  /** Get a single random number from a uniform distribution over [0,1). */
  inline FLOAT_TYPE getUniformRandom();


protected:
  /// Number of random values which are provided
  uint m_numberOfRandomValues;

  /// Array of precomputed random numbers
  FLOAT_TYPE* m_randomValues;

  /// The underlying gaussian random number generator
  GaussRand<FLOAT_TYPE>* m_pGauss;

  /// The index of the last returned random value
  uint m_currentRandomValueIndex;


public:
  inline void moveToNextRandomValue()
    {
      --m_currentRandomValueIndex;
      if ( m_currentRandomValueIndex == 0 )
      {
        m_currentRandomValueIndex = m_numberOfRandomValues - (int) ( (m_numberOfRandomValues/2) * m_pGauss->getUniformRandom() );
      }
    }
};

#include "fast_gauss_rand.cpp"

#endif
