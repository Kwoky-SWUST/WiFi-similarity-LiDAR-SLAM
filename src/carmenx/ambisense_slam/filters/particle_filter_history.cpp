#include <iostream>
#include <cmath>
#include <list>
#include <numeric>
#include <assert.h>


// --------------------------------------------------------------------------
template< class PARTICLE_TYPE >
pf2::ParticleFilterHistory< PARTICLE_TYPE >::
    ParticleFilterHistory()
// --------------------------------------------------------------------------
{
  currentHistory  = NULL;
  previousHistory = NULL;
  m_uFilterSize = 0;
  m_uMostLikelyParticle = (uint) -1; // invalidation
}


// --------------------------------------------------------------------------
template< class PARTICLE_TYPE >
pf2::ParticleFilterHistory< PARTICLE_TYPE >::
    ~ParticleFilterHistory()
// --------------------------------------------------------------------------
{
  delete [] currentHistory;  currentHistory = NULL;
  delete [] previousHistory; previousHistory = NULL;
/*  free( currentHistory );
  free( previousHistory );*/
}


// --------------------------------------------------------------------------
template< class PARTICLE_TYPE >
void pf2::ParticleFilterHistory< PARTICLE_TYPE >::
    addTimeStep( ParticleFilter< PARTICLE_TYPE >& src )
// --------------------------------------------------------------------------
{
  // First, swap previous and current history, or create new ones if not
  // yet created
  if ( currentHistory == NULL )
  {
    m_uFilterSize = src.size();
    currentHistory  = new ParticleHistory[ src.size() ];
    previousHistory = new ParticleHistory[ src.size() ];
  }
  else
  {
    assert( src.size() == m_uFilterSize );
    // Swap current and previous history
    ParticleHistory* tmp = currentHistory;
    currentHistory  = previousHistory;
    previousHistory = tmp;
  }

  PARTICLE_TYPE* particle;
  typedef typename pf2::ParticleFilter< PARTICLE_TYPE >::SampleSet SampleSet;
  SampleSet& particles = src.getParticles();
  for ( uint u = 0; u < src.size(); ++u )
  {
    particle = &( particles[u] );
    assert( particle != NULL );
    if ( particle->parent >= (int)src.size() )
    {
      std::cout << "PROBLEM: size is " << src.size() << ", parent is " << particle->parent << std::endl;
    }
    assert( particle->parent < (int)src.size() );
    assert( particle->parent >= -1 );
    if ( particle->parent >= 0 )
      currentHistory[u] = previousHistory[ particle->parent ];
    currentHistory[u].push_back( *particle );

    // Now set parent to node index itself (will only be changed
    // by resampling
    particle->parent = u;
  }

  int mlp = src.getMostLikelyParticle();
  assert( mlp >= 0 );
  m_uMostLikelyParticle = mlp;
}


// --------------------------------------------------------------------------
template< class PARTICLE_TYPE >
uint pf2::ParticleFilterHistory< PARTICLE_TYPE >::
    getNumberOfTimeSteps() const
// --------------------------------------------------------------------------
{
  if ( currentHistory == NULL )
    return 0;
  else
    return currentHistory[0].size();
}

