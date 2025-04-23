#ifndef AMBISENSE_LOCALIZE__GAUSS_RAND_H
#define AMBISENSE_LOCALIZE__GAUSS_RAND_H

#include <stdlib.h>
#include <math.h>
#include <boost/random.hpp>


/** Pair of gaussian random numbers. Are generated pairwisely because that
 *  is more efficient */
// --------------------------------------------------------------------------
template< class FLOAT_TYPE=double >
class GaussNumberPair
// --------------------------------------------------------------------------
{
  public:
    FLOAT_TYPE value[2];
};



/** Gaussian pseudo-random number generator. Accords to polar form of
 *  Box-Muller transformation.
 *  @author Philipp Vorst
 */
// --------------------------------------------------------------------------
template< class FLOAT_TYPE=double >
class GaussRand
// --------------------------------------------------------------------------
{
public:
  GaussRand<FLOAT_TYPE>( );
  GaussRand<FLOAT_TYPE>( FLOAT_TYPE mean, FLOAT_TYPE sigma );
  virtual ~GaussRand<FLOAT_TYPE>();

  inline void setMeanAndSigma( FLOAT_TYPE mean, FLOAT_TYPE sigma )
  {
    m_mean  = mean;
    m_sigma = sigma;
    m_currentBufferIndex = 0;	// buffer is reset
  }

  inline FLOAT_TYPE getMean() { return m_mean; }

  inline FLOAT_TYPE getSigma() { return m_sigma; }

  /** Get a single random number from a gaussian distribution. */
  FLOAT_TYPE getRandomNumber();

  /** Get a single uniformly distributed random number */
  FLOAT_TYPE getUniformRandom();

  /** Get a pair of random numbers from a gaussian distribution. */
  GaussNumberPair<FLOAT_TYPE> getRandomNumberPair();


protected:
  /// Mean value
  FLOAT_TYPE m_mean;
  /// Standard deviation
  FLOAT_TYPE m_sigma;

  FLOAT_TYPE m_min, m_max, m_diff;

  /// 2.0f / m_diff
  FLOAT_TYPE m_2_diff;

  // Variables for handling the random pair buffer which serves the routine that provides a single random number
  GaussNumberPair<FLOAT_TYPE> m_pair2SingleBuffer;
  int m_currentBufferIndex;

  typedef boost::lagged_fibonacci607 MyRandomGeneratorType;	// fastest!!
//	typedef boost::mt19937 MyRandomGeneratorType;

  /// Boost's random number generator
  MyRandomGeneratorType *m_pGenerator;

  /** Compute ln(x) by formula log_a(x) = log_b(x)/log_b(a) */
  inline FLOAT_TYPE ln( FLOAT_TYPE x )
  {
    return (log10(x) / M_LOG10E);
  }
};

#include "gauss_rand.cpp"

#endif
