// --------------------------------------------------------------------------
template< class FLOAT_TYPE >
FastGaussRand<FLOAT_TYPE>::FastGaussRand( FLOAT_TYPE exp,
                                          FLOAT_TYPE sigma,
                                          uint numberOfRandomValues )
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
    cout << "FastGaussRand: too few random numbers to be generated (" << m_numberOfRandomValues << ")." << endl;
    exit( EXIT_FAILURE );
  }

  m_pGauss = new GaussRand<FLOAT_TYPE>( exp, sigma );
  m_currentRandomValueIndex = m_numberOfRandomValues;
  m_randomValues = (FLOAT_TYPE*) malloc( m_numberOfRandomValues * sizeof(FLOAT_TYPE) );

  for ( uint i = 0; i < m_numberOfRandomValues; i++ )
    m_randomValues[i] = m_pGauss->getRandomNumber();
}


// --------------------------------------------------------------------------
template< class FLOAT_TYPE >
FastGaussRand<FLOAT_TYPE>::~FastGaussRand()
// --------------------------------------------------------------------------
{
  free( m_randomValues ); m_randomValues = NULL;
  delete m_pGauss;        m_pGauss = NULL;
}


// --------------------------------------------------------------------------
template< class FLOAT_TYPE >
inline FLOAT_TYPE FastGaussRand<FLOAT_TYPE>::getUniformRandom()
// --------------------------------------------------------------------------
{
  return m_pGauss->getUniformRandom();
}


// --------------------------------------------------------------------------
template< class FLOAT_TYPE >
inline GaussNumberPair<FLOAT_TYPE> FastGaussRand<FLOAT_TYPE>::getRandomNumberPair()
// --------------------------------------------------------------------------
{
  GaussNumberPair<FLOAT_TYPE> ret;

  moveToNextRandomValue();
  ret.value[0]  = m_randomValues[ m_currentRandomValueIndex ];
  moveToNextRandomValue();
  ret.value[1]  = m_randomValues[ m_currentRandomValueIndex ];

  return ret;
}
