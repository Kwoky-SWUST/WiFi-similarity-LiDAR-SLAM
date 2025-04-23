#include "gauss_rand.h"

using namespace std;


// --------------------------------------------------------------------------
template< class FLOAT_TYPE >
FLOAT_TYPE GaussRand<FLOAT_TYPE>::getUniformRandom()
// --------------------------------------------------------------------------
{
  return ( (*m_pGenerator)() - m_min) / m_diff;
}


// --------------------------------------------------------------------------
template< class FLOAT_TYPE >
GaussNumberPair<FLOAT_TYPE> GaussRand<FLOAT_TYPE>::getRandomNumberPair()
// --------------------------------------------------------------------------
{
  double x1, x2, w, t;
  GaussNumberPair<FLOAT_TYPE> ret;

  do {
    x1 = ( (*m_pGenerator)() - m_min) * m_2_diff - 1.0;
    x2 = ( (*m_pGenerator)() - m_min) * m_2_diff - 1.0;
    w  = x1 * x1;
    w += x2 * x2;
  } while ( w >= 1.0 );

  w = sqrt( (-2.0 * ln( w ) ) / w );
  t = w * m_sigma;
  ret.value[0]  = x1 * t;
  ret.value[0] += m_mean;
  ret.value[1]  = x2 * t;
  ret.value[1] += m_mean;

  return ret;
}


// --------------------------------------------------------------------------
template< class FLOAT_TYPE >
FLOAT_TYPE GaussRand<FLOAT_TYPE>::getRandomNumber()
// --------------------------------------------------------------------------
{
  if ( m_currentBufferIndex == 0 )
  {
    m_pair2SingleBuffer = getRandomNumberPair();
    m_currentBufferIndex = 1;
    return m_pair2SingleBuffer.value[0];
  }
  else
  {
    m_currentBufferIndex = 0;
    return m_pair2SingleBuffer.value[1];
  }
}


// --------------------------------------------------------------------------
template< class FLOAT_TYPE >
GaussRand<FLOAT_TYPE>::GaussRand( FLOAT_TYPE mean, FLOAT_TYPE sigma )
// --------------------------------------------------------------------------
{
  // Initialise buffer and set ext/ sigma locally
  setMeanAndSigma( mean, sigma );

  // Create objects needed
  m_pGenerator = new MyRandomGeneratorType(time(0));
  m_min = m_pGenerator->min();
  m_max = m_pGenerator->max();
  m_diff = (m_max - m_min);
  m_2_diff = 2.0f / m_diff; // pre-calculate for efficiency reasons

  srand48( (unsigned) time(NULL) );
}


// --------------------------------------------------------------------------
template< class FLOAT_TYPE >
GaussRand<FLOAT_TYPE>::GaussRand( )
// --------------------------------------------------------------------------
{
  GaussRand( 0, 1 );
}


// --------------------------------------------------------------------------
template< class FLOAT_TYPE >
    GaussRand<FLOAT_TYPE>::~GaussRand( )
// --------------------------------------------------------------------------
{
  delete m_pGenerator; m_pGenerator = NULL;
}
