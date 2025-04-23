// #include "fast_uniform_rand.h"
#include <iostream>


// --------------------------------------------------------------------------
template< class FLOAT_TYPE >
FastUniformRand<FLOAT_TYPE>::
    FastUniformRand( unsigned long numberOfRandomValues )
// --------------------------------------------------------------------------
{
  /* Provide 17 more random values than requested. This is because 17 is
     a nice prime, so even for the case that as many random values are
     needed as are stored, in the following iterations of any algorithm
     these random values are not assigned to the same variables with
     higher probability. */
  m_numberOfRandomValues = numberOfRandomValues + 17;
  if ( m_numberOfRandomValues < 1 )
  {
    std::cout << "FastUniformRand: too few random numbers to be generated (" << m_numberOfRandomValues << ")." << std::endl;
    exit( -1 );
  }

  m_pUniform = new UniformRand<FLOAT_TYPE>();      // Create underlying uniform random number generator
  m_randomValues = new FLOAT_TYPE[ m_numberOfRandomValues ];  // Reserve space for precomputed numbers

  // Generate random numbers and store them
  for ( unsigned long i = 0; i < m_numberOfRandomValues; ++i )
  {
    m_randomValues[i] = m_pUniform->getRandomNumber();
  }

  m_currentRandomValueIndex = m_numberOfRandomValues;
  m_currentRandomValue      = m_randomValues + m_currentRandomValueIndex;
}


// --------------------------------------------------------------------------
template< class FLOAT_TYPE >
FastUniformRand<FLOAT_TYPE>::
    ~FastUniformRand( )
// --------------------------------------------------------------------------
{
  delete [] m_randomValues;
  delete m_pUniform;
}


// --------------------------------------------------------------------------
template< class FLOAT_TYPE >
inline void FastUniformRand<FLOAT_TYPE>::
    moveToNextRandomValue()
// --------------------------------------------------------------------------
{
//       --m_currentRandomValueIndex;
  --m_currentRandomValue;

  // Array beginning reached?
  if ( m_currentRandomValue == m_randomValues )
  {
    m_currentRandomValueIndex = m_numberOfRandomValues - (int) ( (m_numberOfRandomValues / 2) * m_pUniform->getRandomNumber() );
//        m_currentRandomValueIndex = (int) ( (m_numberOfRandomValues-1) * m_pUniform->getRandomNumber() );
    m_currentRandomValue = m_randomValues + m_currentRandomValueIndex;
  }
}
