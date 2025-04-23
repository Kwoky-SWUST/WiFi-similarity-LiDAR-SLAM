
// --------------------------------------------------------------------------
template< class FLOAT_TYPE >
FLOAT_TYPE UniformRand< FLOAT_TYPE >::getRandomNumber()
// --------------------------------------------------------------------------
{
  return ( (*m_pGenerator)() - m_min) * m_1_diff;
}


// --------------------------------------------------------------------------
template< class FLOAT_TYPE >
FLOAT_TYPE UniformRand< FLOAT_TYPE >::
    getRandomNumber( FLOAT_TYPE a, FLOAT_TYPE intervalLength )
// --------------------------------------------------------------------------
{
  return ((*m_pGenerator)() - m_min) * ( intervalLength * m_1_diff ) + a;
}


// --------------------------------------------------------------------------
template< class FLOAT_TYPE >
    int UniformRand<FLOAT_TYPE>::getRandomInteger( int a, int intervalLength )
// --------------------------------------------------------------------------
{
  return (int) getRandomNumber( a, a+intervalLength );
}


// --------------------------------------------------------------------------
template< class FLOAT_TYPE >
    UniformRand< FLOAT_TYPE >::UniformRand( )
// --------------------------------------------------------------------------
{
  m_pGenerator = new MyRandomGeneratorType(time(0));
  m_min = m_pGenerator->min();
  m_max = m_pGenerator->max();
//  cout << "MIN: " << m_min << "  MAX: " << m_max << endl;
  m_diff = (m_max - m_min);
  m_int_diff = (uint)(m_max - m_min);
  m_1_diff = 1.0f / m_diff;
}


// --------------------------------------------------------------------------
template< class FLOAT_TYPE >
    UniformRand< FLOAT_TYPE >::~UniformRand( )
// --------------------------------------------------------------------------
{
  delete m_pGenerator;
}
