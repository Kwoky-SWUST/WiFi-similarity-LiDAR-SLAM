#include <carmen/carmen.h>
#include <carmen/global.h>
//
#include <assert.h>
#include <iostream>
#include <list>
//
#include <utils/text/colors.h>
//
#include "mcl_random/fast_uniform_rand.h"
#include "mcl_random/gauss_rand.h"


#define PF_DEBUG std::cout << "debug: " << __LINE__


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
ParticleFilter<PARTICLE, WEIGHT>::
    ParticleFilter( const int size, const FastGaussRand< WEIGHT > * generator,
                    const char *name )
// --------------------------------------------------------------------------
{
  if ( name != NULL )
  {
    m_name = (char*) malloc( strlen(name) + 1);
    strcpy( m_name, name );
    if ( ! m_name )
    {
      std::cerr << "FATAL ERROR: Could not reserve memory in " << __FILE__ << ", line " << __LINE__ << std::endl;
      exit( EXIT_FAILURE );
    }
  }
  else
    m_name = NULL;

  m_pfState.size    = 0;
  m_pfState.maxSize = size;
//   m_pUniformDistribution = new FastUniformRand< WEIGHT >( 12345 + m_pfState.maxSize );
  m_pGaussGenerator = (FastGaussRand< WEIGHT >*) generator;

  // Create arrays for actual and temporarily used sample data
  m_particles       = new PARTICLE[ m_pfState.maxSize ];
  m_particlesTemp   = new PARTICLE[ m_pfState.maxSize ];
  m_particlesBackup = new PARTICLE[ m_pfState.maxSize ];

  m_vKLDSamplingBins.clear();
  m_vKLDSamplingBins.resize( 0, false );
  m_dKLDEpsilon = 0.05;
  m_iKLBounds = NULL;

  m_pfState.mostLikelyParticle = -1; // invalidation as initialization

  m_pfState.sumOfWeights       = 0.0;
  m_pfState.maxWeight = 0.0;
  m_pfState.minWeight = INFINITY;
  reset();
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
ParticleFilter<PARTICLE, WEIGHT>::
ParticleFilter( ParticleFilter<PARTICLE, WEIGHT> & source,
                const FastGaussRand< WEIGHT > * generator )
// --------------------------------------------------------------------------
{
  // Copy the particle filter name
  if ( source.getName() != NULL )
  {
    m_name    = (char*) malloc( strlen(source.getName()) + 1 );
    strcpy( m_name, source.getName() );
    if ( ! m_name )
    {
      std::cerr << "FATAL ERROR: Could not reserve memory in " << __FILE__ << ", line " << __LINE__ << std::endl;
      exit( EXIT_FAILURE );
    }
  }
  else
    m_name = NULL;

  m_pfState.size    = source.size();
  m_pfState.maxSize = m_pfState.size;
//   m_pUniformDistribution = new FastUniformRand< WEIGHT >( 123456 + m_pfState.maxSize );
  m_pGaussGenerator = (FastGaussRand< WEIGHT >*) generator;

  // Create arrays for actual and temporarily used sample data
  m_particles     = new PARTICLE[ m_pfState.maxSize ];
  m_particlesTemp = new PARTICLE[ m_pfState.maxSize ];

  // Copy all particles
  memcpy( m_particles, source.getParticles(), m_pfState.size );

  m_pfState.sumOfWeights       = source.getStatisticsEta();
  m_pfState.maxWeight = source.getStatisticsMaxWeight();
  m_pfState.minWeight = source.getStatisticsMinWeight();
  reset();
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
ParticleFilter<PARTICLE, WEIGHT>::
~ParticleFilter()
// --------------------------------------------------------------------------
{
  free( m_name );
  if ( m_iKLBounds != NULL )
    delete [] m_iKLBounds;
  delete [] m_particles;
  delete [] m_particlesBackup;
  delete [] m_particlesTemp;
//   delete m_pUniformDistribution;
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
void ParticleFilter<PARTICLE, WEIGHT>::
addGaussianNoise( const WEIGHT translationStdDev,
                  const WEIGHT orientationStdDev )
// --------------------------------------------------------------------------
{
  GaussNumberPair< WEIGHT > pair;

  if ( translationStdDev > 0 )
  {
    for ( int i = 0; i < m_pfState.size; ++i )
    {
      pair = m_pGaussGenerator->getRandomNumberPair();

      m_particles[ i ].x += pair.value[0] * translationStdDev;
      m_particles[ i ].y += pair.value[1] * translationStdDev;
    }
  }

  if ( orientationStdDev > 0 )
  {
    for ( int i = 0; i < m_pfState.size; ++i )
    {
      if ( ! (i%2) )
        pair = m_pGaussGenerator->getRandomNumberPair();

      m_particles[i].theta += pair.value[i % 2] * orientationStdDev;
    }
  }
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
void ParticleFilter<PARTICLE, WEIGHT>::
    initializeGaussian( const PARTICLE & defaultPosition,
                        const WEIGHT     translationStdDev,
                        const WEIGHT     orientationStdDev )
// --------------------------------------------------------------------------
{
  reset();
  m_pfState.size = m_pfState.maxSize;

  const WEIGHT _w = 1.0 / m_pfState.size;
  for ( int i = 0; i != m_pfState.size; ++i )
  {
    m_particles[i] = defaultPosition;
    m_particles[i].weight = _w;
#ifdef HAVE_LOG_WEIGHTS
    m_particles[i].log_weight = log( _w );
#endif
    m_particles[i].accumulated_weight = ( i + 1 ) * _w;
  }

  m_pfState.normalized = true;

  addGaussianNoise( translationStdDev, orientationStdDev );
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
int ParticleFilter<PARTICLE, WEIGHT>::
    getEffectiveSampleSize()
// --------------------------------------------------------------------------
{
  if ( ! m_pfState.essComputed )
  {
    double _sum = 0.0;
    const PARTICLE *_pSampleSetEnd = m_particles + m_pfState.size;
    PARTICLE *_pSample;

    for ( _pSample = m_particles;
          _pSample != _pSampleSetEnd;
          ++_pSample )
    {
      _sum += carmen_square( _pSample->weight );
    }

    m_pfState.effectiveSampleSize = (int) (1.0 / _sum + 0.5);
    m_pfState.essComputed = true;
  }

  // NOTE: In the following line, the 0.5 is only required for rounding
  return m_pfState.effectiveSampleSize;
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
void ParticleFilter<PARTICLE, WEIGHT>::
    reset()
// --------------------------------------------------------------------------
{
  m_pfState.centroidComputed = false;
  m_pfState.normalized       = false;
  m_pfState.varianceComputed = false;
  m_pfState.essComputed      = false;
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
void ParticleFilter<PARTICLE, WEIGHT>::
    resetCenterAndVariance()
// --------------------------------------------------------------------------
{
  m_pfState.centroidComputed = false;
  m_pfState.varianceComputed = false;
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
void ParticleFilter<PARTICLE, WEIGHT>::
    clear()
// --------------------------------------------------------------------------
{
  m_pfState.size = 0;
  reset();
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
void ParticleFilter<PARTICLE, WEIGHT>::
    copyFrom( const ParticleFilter<PARTICLE, WEIGHT> * src )
// --------------------------------------------------------------------------
{
  m_pfState.size = min( m_pfState.maxSize, src->size() );
  reset();
  for ( int i = 0; i != m_pfState.maxSize; ++i )
  {
    m_particles[i] = src->getParticles()[i];
  }
  normalize();
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
inline const PARTICLE* ParticleFilter<PARTICLE, WEIGHT>::
    getParticles() const
// --------------------------------------------------------------------------
{
  return m_particles;
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
PARTICLE & ParticleFilter<PARTICLE, WEIGHT>::
    getCenter()
// --------------------------------------------------------------------------
{
  if ( ! m_pfState.centroidComputed )
  {
    // Set centroid zero
    m_pfState.center.x = m_pfState.center.y = m_pfState.center.theta = 0;

    WEIGHT _theta_x = 0, _theta_y = 0;

    if ( ! m_pfState.normalized )
    {
      normalize();
#if 0
      // PARTICLE set is not normalized, so we have to divide by sum of
      // weights afterwards

      WEIGHT _w, acc_w = 0;

      for ( PARTICLE *_pSample = m_particles;
            _pSample != m_particles + m_pfState.size;
            ++_pSample )
      {
        _w = _pSample->weight;
        acc_w += _w;

        m_pfState.center.x += _w * _pSample->x;
        m_pfState.center.y += _w * _pSample->y;

        _theta_x += _w * cos(_pSample->theta);
        _theta_y += _w * sin(_pSample->theta);
      }

      m_pfState.center.x /= acc_w;
      m_pfState.center.y /= acc_w;

      _theta_x   /= acc_w;
      _theta_y   /= acc_w;
      m_pfState.center.theta = atan2f( _theta_y, _theta_x );
#endif
    }
//     else
    {
      // Nicely, the sample set is normalized, we can save a few
      // computations

      WEIGHT _w;

      for ( PARTICLE *_pSample = m_particles;
            _pSample != m_particles + m_pfState.size;
            ++_pSample )
      {
        _w = _pSample->weight;

        m_pfState.center.x += _pSample->x * _w;
        m_pfState.center.y += _pSample->y * _w;

        _theta_x += cos(_pSample->theta) * _w;
        _theta_y += sin(_pSample->theta) * _w;
      }

      m_pfState.center.theta = atan2( _theta_y, _theta_x );
    }

    m_pfState.centroidComputed = true;
  }

  return m_pfState.center;
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
PARTICLE & ParticleFilter<PARTICLE, WEIGHT>::
    getVariance()
// --------------------------------------------------------------------------
{
  if ( ! m_pfState.varianceComputed )
  {
    m_pfState.variance.x     = 0;
    m_pfState.variance.y     = 0;
    m_pfState.variance.theta = 0;
    m_pfState.xyCovariance   = 0;

    if ( ! m_pfState.centroidComputed || ! m_pfState.normalized )
    {
      std::cerr << "getVariance(): You need to normalize and compute the centroid first!" << std::endl;
      exit( EXIT_FAILURE );
    }
    else
    {
      WEIGHT theta_diff, _w;
      int multiplier;

      WEIGHT TWO_PI    = 2*M_PI;
      WEIGHT BY_TWO_PI = 1/(2*M_PI);

      for ( PARTICLE *_pSample = m_particles;
            _pSample != m_particles + m_pfState.size;
            ++_pSample )
      {
        _w = _pSample->weight;
        m_pfState.variance.x     += carmen_square( _pSample->x - m_pfState.center.x ) * _w;
        m_pfState.variance.y     += carmen_square( _pSample->y - m_pfState.center.y ) * _w;

        theta_diff = _pSample->theta - m_pfState.center.theta;

        multiplier = (int)(theta_diff * BY_TWO_PI);
        theta_diff = theta_diff - multiplier*TWO_PI;
        if (theta_diff >= M_PI)
          theta_diff -= TWO_PI;
        else if (theta_diff < -M_PI)
          theta_diff += TWO_PI;

        m_pfState.variance.theta += carmen_square( theta_diff ) * _w;
        m_pfState.xyCovariance   += ( _pSample->x - m_pfState.center.x ) * ( _pSample->y - m_pfState.center.y ) * _w;
      }
    }

    m_pfState.varianceComputed = true;
  }

  return m_pfState.variance;
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
inline WEIGHT ParticleFilter<PARTICLE, WEIGHT>::
    getXYCovariance()
// --------------------------------------------------------------------------
{
  if ( ! m_pfState.varianceComputed )
    std::cerr << "WARNING in ParticleFilter::getXYCovariance: Variance has not been computed yet!" << std::endl;

  return m_pfState.xyCovariance;
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
void ParticleFilter<PARTICLE, WEIGHT>::
    normalizeWithLogWeights()
// --------------------------------------------------------------------------
{
#ifdef HAVE_LOG_WEIGHTS
  WEIGHT    _log_w, _max = -INFINITY;
  PARTICLE* _pSample = m_particles;

  m_pfState.minWeight = INFINITY;

  for ( int i = 0; i < m_pfState.size; ++i )
  {
    _log_w = _pSample->log_weight;
    assert( ! isinf( _log_w ) );
    assert( ! isnan( _log_w ) );

    if ( _log_w > _max )
    {
      _max = _log_w;
    }
    ++_pSample;
  }

//   std::cout << "DEBUG: _max: " << _max << endl;

  _pSample = m_particles;
  for ( int i = 0; i < m_pfState.size; ++i )
  {
    _pSample->log_weight -= _max;
    _pSample->weight      = exp( _pSample->log_weight );
//     std::cout << "DEBUG: LOGW: " << _pSample->log_weight << ",    W: " << _pSample->weight << endl;

    ++_pSample;
  }
#endif

  normalize();
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
void ParticleFilter<PARTICLE, WEIGHT>::
    normalize()
// --------------------------------------------------------------------------
{
  WEIGHT eta = 0.0, _w, _max = 0.0;
  int    zeroCount = 0;
  PARTICLE *_pSample = m_particles;

  m_pfState.minWeight = INFINITY;

  for ( int i = 0; i < m_pfState.size; ++i )
  {
    _w = _pSample->weight;
    assert( ! isinf( _w ) );

    if ( _w > 0.0 )
    {
      if ( _w > _max )
      {
        _max = _w;
        m_pfState.mostLikelyParticle = i;
      }

      eta += _w;
    }
    else
      ++zeroCount;

    if ( _w < m_pfState.minWeight )
      m_pfState.minWeight = _w;

    ++_pSample;
  }

//   std::cout << " ZERO: " << zeroCount << " Size: " << size() << std::endl;


  // Remember accumulated belief for statistical purposes
  m_pfState.sumOfWeights = eta;

  // Remember maximum of all beliefs for statistical purposes
  m_pfState.maxWeight = _max;
  WEIGHT accumulated_weight = 0.0;

  // Set pointer to first element of sample array
  _pSample = m_particles;

  if ( eta > 0.0 )
  {
    WEIGHT inverse_eta = 1.0 / eta;
#ifdef HAVE_LOG_WEIGHTS
    WEIGHT log_normalizer_inverted = -log( eta );
#endif

    for ( int i = 0; i < m_pfState.size; ++i )
    {
      _pSample->weight *= inverse_eta;
      accumulated_weight += _pSample->weight;
#ifdef HAVE_LOG_WEIGHTS
      _pSample->log_weight += log_normalizer_inverted;
#endif

      _pSample->accumulated_weight = accumulated_weight;
      ++_pSample;  // Go to next sample
    }

    m_pfState.maxWeight *= inverse_eta;
    m_pfState.minWeight *= inverse_eta;
  }
  else
  {
    std::cerr << "ParticleFilter: WARNING: All particles accumulate to belief of zero. Setting all sample importance values 1/n." << std::endl;

    m_pfState.maxWeight = 1.0 / m_pfState.size;
    for ( int i = 0; i < m_pfState.size; ++i )
    {
      _pSample->weight     = m_pfState.maxWeight;
#ifdef HAVE_LOG_WEIGHTS
      _pSample->log_weight = log( _pSample->weight );
#endif
      accumulated_weight += _pSample->weight;
      _pSample->accumulated_weight = accumulated_weight;

      _pSample++;  // Go to next sample
    }
  }

  m_pfState.normalized       = true;
  m_pfState.centroidComputed = false;
  m_pfState.varianceComputed = false;
  m_pfState.essComputed      = false;
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
void ParticleFilter<PARTICLE, WEIGHT>::
    kldInitialize( double mapMinX,  double mapMaxX,
                   double mapMinY,  double mapMaxY,
                   double binWidth, double binAngle,
                   int    minimalNumberOfParticles,
                   double delta,    double epsilon
                 )
// --------------------------------------------------------------------------
{
  m_dKLDSamplingBinWidth = binWidth;
  m_dKLDSamplingBinAngle = binAngle;

  m_dKLDSamplingMinX = mapMinX;
  m_dKLDSamplingMinY = mapMinY;

  assert( mapMaxX > mapMinX );
  assert( mapMaxY > mapMinY );
  assert( m_dKLDSamplingBinWidth > 0.0 );
  assert( m_dKLDSamplingBinAngle > 0.0 );

  m_iKLDSamplingMapWidth     = (int)((mapMaxX - mapMinX) / m_dKLDSamplingBinWidth) + 1;
  m_iKLDSamplingMapHeight    = (int)((mapMaxY - mapMinY) / m_dKLDSamplingBinWidth) + 1;
  m_iKLDSamplingOrientations = (int)(2*M_PI / m_dKLDSamplingBinAngle) + 1;

  m_iKLDMinNumberOfParticles = minimalNumberOfParticles;

  m_dKLDSamplingDelta = delta;
  m_dKLDEpsilon       = epsilon;

  assert( m_dKLDSamplingDelta > 0 );
  assert( m_dKLDEpsilon       > 0 );

  m_vKLDSamplingBins.resize( m_iKLDSamplingMapWidth * m_iKLDSamplingMapHeight * m_iKLDSamplingOrientations, false );

  // Find upper (1-delta) quantile of the standard normal N(0,1) distribution
  double p_accumulated = 0.0;
  double p_x;
  double f = 1.0 / ( sqrt(2*M_PI) * 1.0 /*=sigma*/ );
  double termination_p = (1 - m_dKLDSamplingDelta) / f;
  double integration_step = 0.05;

  // For integration, start with a large x far beyond the relevant
  // standard deviations
  {
    double x;
    for ( x = 100.0; p_accumulated < termination_p; x -= integration_step )
    {
      p_x = exp( - 0.5 * x*x ) * integration_step;
      p_accumulated += p_x;
    }
    m_dKLDSamplingUpperQuantile = x + integration_step/2;
  }

//   cout << "debug: m_dKLDSamplingUpperQuantile = " << m_dKLDSamplingUpperQuantile << endl;


  // Precompute KL bounds /////////////////////////////////
  if ( m_iKLBounds != NULL )
    delete m_iKLBounds;
  m_iKLBounds = new int[ m_pfState.maxSize ];
  m_iKLBounds[0] = 0;

  double a, b;
  for ( int k = 1; k < m_pfState.maxSize; ++k )
  {
    a = 2 / (9*k);
    b = 1.0 - a + sqrt(a) * m_dKLDSamplingUpperQuantile;
    b *= (b*b); // b^3
    m_iKLBounds[k] = (int) ( k/(2*m_dKLDEpsilon) * b );

//     cout << "debug: kl bound " << k << ": " << m_iKLBounds[k] << endl;
  }
#if 0
  // NOTE: original formula/computation was the following, but it does not
  //       make sense for k=1 after drawing the first sample!
    a = 2 / (9*(k-1));
    b = 1.0 - a + sqrt(a) * m_dKLDSamplingUpperQuantile;
    b *= (b*b); // b^3
    m_iKLBounds[k] = (int) ( (k-1)/(2*epsilon) * b );
#endif
  // //////////////////////////////////////////////////////

}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
void ParticleFilter<PARTICLE, WEIGHT>::
    kldSample( LikelihoodFunction likelihoodFunction, void* sensorData )
// --------------------------------------------------------------------------
{
  int angle_multiplier;
  std::list< int > bins_with_support;

  int k = 1; // number of bins with support
  int n_samples = 0; // number of sampled particles so far
  int kl_bound = 1; // initial bound
  int bx, by, btheta; // bin indices of new sample
  int bin_index;

  register WEIGHT accumulated_weight = 0;

  // Set sample pointer to first sample of array...
  register PARTICLE * _pWhichParticle = m_particles;
  register PARTICLE * _pNewSample     = m_particlesTemp;

  ++m_pfState.numberOfResamplings;

  assert( isNormalized() );
  assert( m_iKLDSamplingMapWidth     > 0 );
  assert( m_iKLDSamplingMapHeight    > 0 );
  assert( m_iKLDSamplingOrientations > 0 );

  // Draw random samples and apply likelihood function until
  // KL bound is reached
  int samples_outside_bins = 0;
  do
  {
    // Draw new sample
    _pWhichParticle = m_particles + getParticleRandomly(); //&( m_particles[ debug_drawn_index ] );

    // NOTE: memcpy works only for samples without dynamic elements,
    // e.g. trajectories. Therefore we have switched to the (potentially
    // slower) direct assignment of samples
    *_pNewSample = *_pWhichParticle;
//     memcpy( _pNewSample, _pWhichParticle, sizeof(PARTICLE) );

    // Evaluate likelihood function
    _pNewSample->weight = likelihoodFunction( _pNewSample, sensorData );
#ifdef HAVE_LOG_WEIGHTS
    _pNewSample->log_weight = log( _pNewSample->weight );
#endif
    accumulated_weight += _pNewSample->weight;
    _pNewSample->accumulated_weight = accumulated_weight;

    // Normalize angle for computation of bin indices
    if ( _pNewSample->theta < 0 || _pNewSample->theta >= 2*M_PI )
    {
      angle_multiplier    = (int) (_pNewSample->theta / (2*M_PI));

      // Correct ange multiplier for the case that _pNewSample->theta < 0
      if ( _pNewSample->theta < 0 )
        --angle_multiplier;

      _pNewSample->theta -= ( angle_multiplier * (2 * M_PI) );
    }

    // Compute bin indices
    bx     = (int) ((_pNewSample->x - m_dKLDSamplingMinX) / m_dKLDSamplingBinWidth );
    by     = (int) ((_pNewSample->y - m_dKLDSamplingMinY) / m_dKLDSamplingBinWidth );
    btheta = (int) ( _pNewSample->theta / m_dKLDSamplingBinAngle );

//     cout << "  Sample " << n_samples << ": theta=" << _pNewSample->theta << ", x=" << _pNewSample->x << ", bx=" << bx << ", y=" << _pNewSample->y << ", by=" << by << ", btheta=" << btheta << ", bin_index=" << bin_index << endl;

    // Check whether new sample falls into empty bin
    if (    bx >= 0     && bx < m_iKLDSamplingMapWidth
         && by >= 0     && by < m_iKLDSamplingMapHeight
         && btheta >= 0 && btheta < m_iKLDSamplingOrientations )
    {
      bin_index = bx + by*m_iKLDSamplingMapWidth + btheta * ( m_iKLDSamplingMapWidth * m_iKLDSamplingMapHeight );

      if ( ! m_vKLDSamplingBins[ bin_index ]
           && kl_bound < m_pfState.maxSize )
      {
        m_vKLDSamplingBins[ bin_index ] = true;
        ++k;
        bins_with_support.push_back( bin_index );

//         cout << "debug: " << k << " bins with support, bin index " << bin_index << " (" << m_iKLDSamplingMapWidth*m_iKLDSamplingMapHeight*m_iKLDSamplingOrientations << ")" << endl;

        kl_bound = m_iKLBounds[ k ];
//         cout << color::cyan << "  New KL bound: " << kl_bound << color::normal << endl;

        if ( kl_bound > m_pfState.maxSize )
        {
          kl_bound = m_pfState.maxSize;
        }
        else if ( kl_bound < m_iKLDMinNumberOfParticles )
        {
          kl_bound = m_iKLDMinNumberOfParticles;
        }
      }

      ++n_samples;
    }
    else
    {
      ++samples_outside_bins;
    }

    ++_pNewSample;

  } while ( n_samples < kl_bound
            && n_samples + samples_outside_bins < m_pfState.maxSize );

//   cout << "debug: " << k << " bins with support, n_samples=" << n_samples << ", samples_outside_bins=" << samples_outside_bins << endl;

  m_pfState.size = n_samples + samples_outside_bins;
  m_iKLDNumberOfBinsWithSupport = k;

  double duration = -carmen_get_time();
  // Reset sampling bins.
  // TODO: Potential optimization: memset with 0
  for ( std::list<int>::iterator it = bins_with_support.begin();
        it != bins_with_support.end();
        ++it )
  {
    m_vKLDSamplingBins[ *it ] = false;
  }
  duration += carmen_get_time();

  // TODO: Optimizable, too. (Use one static array, toggle flag number)
  bins_with_support.clear();

  // Swap actual and temporary data
  PARTICLE *tmp    = m_particles;
  m_particles      = m_particlesTemp;
  m_particlesTemp  = tmp;

  // Normalize ////////////////////////
  // (done here because norm. factor is known)
  WEIGHT normalization_factor = 1.0 / accumulated_weight;
#ifdef HAVE_LOG_WEIGHTS
  WEIGHT log_normalizer_inverted = -log( accumulated_weight );
#endif
  for ( int i = 0; i < m_pfState.size; ++i )
  {
    m_particles[i].weight             *= normalization_factor;
#ifdef HAVE_LOG_WEIGHTS
    m_particles[i].log_weight += log_normalizer_inverted;
#endif
    m_particles[i].accumulated_weight *= normalization_factor;
  }
  m_pfState.sumOfWeights = normalization_factor;
  // //////////////////////////////////

  // Postconditions
  m_pfState.normalized       = true;
  m_pfState.centroidComputed = false;
  m_pfState.varianceComputed = false;

  m_pfState.effectiveSampleSize = m_pfState.size;
  m_pfState.essComputed          = true;

}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
void ParticleFilter<PARTICLE, WEIGHT>::
    resample( const int numberOfSamples, const WEIGHT weight )
// --------------------------------------------------------------------------
{
  // NOTE: This implementation goes back to the CARMEN resampling
  //       method used in localizecore.c:carmen_localize_resample(...)

  register       WEIGHT position, step_size;
  register const WEIGHT _newSampleWeight = weight;
  register       WEIGHT accumulated_weight = 0;
#ifdef HAVE_LOG_WEIGHTS
  register const WEIGHT _newSampleLogWeight = log( _newSampleWeight );
#endif

  // Set sample pointer to first sample of array...
  register PARTICLE * _pWhichParticle = m_particles;
  register PARTICLE * _pNewSample     = m_particlesTemp;

  ++m_pfState.numberOfResamplings;
  assert( m_pfState.normalized );

  // Although the sample set is normalized, we have to use the last
  // accumulated weight because of floating point number problems
  // (accumulated weight may be close to, but unequal to 1.0)
  const WEIGHT weight_sum = m_particles[ m_pfState.size-1 ].accumulated_weight;

  // Choose random starting position for low-variance walk
  position = carmen_uniform_random( 0, weight_sum );
  step_size = weight_sum / m_pfState.size;

  // Draw num_particles random samples
  for ( int i = 0; i < numberOfSamples; i++ )
  {
    position += step_size;
    if ( position >= weight_sum )
    {
      position -= weight_sum;
      _pWhichParticle = m_particles;
    }

    while ( position > _pWhichParticle->accumulated_weight )
      ++_pWhichParticle;

    *_pNewSample = *_pWhichParticle;
//     memcpy( _pNewSample, _pWhichParticle, sizeof(PARTICLE) );

    _pNewSample->weight     = _newSampleWeight;
#ifdef HAVE_LOG_WEIGHTS
    _pNewSample->log_weight = _newSampleLogWeight;
#endif

    accumulated_weight += _newSampleWeight;
    _pNewSample->accumulated_weight = accumulated_weight;
    ++_pNewSample;
  }

  // Swap actual and temporary data
  PARTICLE *tmp    = m_particles;
  m_particles      = m_particlesTemp;
  m_particlesTemp  = tmp;

  m_pfState.size = numberOfSamples;

  // Postconditions
  m_pfState.normalized       = true;
  m_pfState.centroidComputed = false;
  m_pfState.varianceComputed = false;

  m_pfState.effectiveSampleSize = m_pfState.size;
  m_pfState.essComputed      = true;
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
void ParticleFilter<PARTICLE, WEIGHT>::
    resampleWithCrossover( const int numberOfSamples, const WEIGHT weight )
// --------------------------------------------------------------------------
{
  // NOTE: This implementation goes back to the CARMEN resampling
  //       method used in localizecore.c:carmen_localize_resample(...)

  register       WEIGHT position1, position2, step_size;
  register const WEIGHT _newSampleWeight = weight;
  register       WEIGHT accumulated_weight = 0;
#ifdef HAVE_LOG_WEIGHTS
  register const WEIGHT _newSampleLogWeight = log( _newSampleWeight );
#endif

  // Set sample pointer to first sample of array...
  register PARTICLE * _pWhichParticle1 = m_particles;
  register PARTICLE * _pWhichParticle2 = m_particles;
  register PARTICLE * _pNewSample     = m_particlesTemp;

  ++m_pfState.numberOfResamplings;
  assert( m_pfState.normalized );

  // Although the sample set is normalized, we have to use the last
  // accumulated weight because of floating point number problems
  // (accumulated weight may be close to, but unequal to 1.0)
  const WEIGHT weight_sum = m_particles[ m_pfState.size-1 ].accumulated_weight;

  // Choose random starting position for low-variance walk
  position1 = carmen_uniform_random( 0, weight_sum );
  position2 = carmen_uniform_random( 0, weight_sum );
  step_size = weight_sum / m_pfState.size;

  WEIGHT cx, cy;

  // Draw num_particles random samples
  for ( int i = 0; i < numberOfSamples; i++ )
  {
    // Sample first particle
    position1 += step_size;
    if ( position1 >= weight_sum )
    {
      position1 -= weight_sum;
      _pWhichParticle1 = m_particles;
    }

    while ( position1 > _pWhichParticle1->accumulated_weight )
      ++_pWhichParticle1;

    // Sample second particle
    position2 += step_size;
    if ( position2 >= weight_sum )
    {
      position2 -= weight_sum;
      _pWhichParticle2 = m_particles;
    }

    while ( position2 > _pWhichParticle2->accumulated_weight )
      ++_pWhichParticle2;
/*
    wf = 1.0 / ( _pWhichParticle1->weight + _pWhichParticle2->weight );
    _pNewSample->x = wf * ( _pWhichParticle1->x * _pWhichParticle1->weight + _pWhichParticle2->x * _pWhichParticle2->weight );
    _pNewSample->y = wf * ( _pWhichParticle1->y * _pWhichParticle1->weight + _pWhichParticle2->y * _pWhichParticle2->weight );

    cx  = cos( _pWhichParticle1->theta ) * _pWhichParticle1->weight;
    cx += cos( _pWhichParticle2->theta ) * _pWhichParticle2->weight;
    cy  = sin( _pWhichParticle1->theta ) * _pWhichParticle1->weight;
    cy += sin( _pWhichParticle2->theta ) * _pWhichParticle2->weight;*/

    _pNewSample->x = 0.5 * ( _pWhichParticle1->x + _pWhichParticle2->x );
    _pNewSample->y = 0.5 * ( _pWhichParticle1->y + _pWhichParticle2->y );

    cx  = cos( _pWhichParticle1->theta );
    cx += cos( _pWhichParticle2->theta );
    cy  = sin( _pWhichParticle1->theta );
    cy += sin( _pWhichParticle2->theta );
    _pNewSample->theta = atan2( cy, cx );

    _pNewSample->weight             = _newSampleWeight;
#ifdef HAVE_LOG_WEIGHTS
    _pNewSample->log_weight         = _newSampleLogWeight;
#endif

    accumulated_weight += _newSampleWeight;
    _pNewSample->accumulated_weight = accumulated_weight;
    ++_pNewSample;
  }

  // Swap actual and temporary data
  PARTICLE *tmp    = m_particles;
  m_particles      = m_particlesTemp;
  m_particlesTemp  = tmp;

  m_pfState.size = numberOfSamples;

  // Postconditions
  m_pfState.normalized       = true;
  m_pfState.centroidComputed = false;
  m_pfState.varianceComputed = false;

  m_pfState.effectiveSampleSize = m_pfState.size;
  m_pfState.essComputed      = true;
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
void ParticleFilter<PARTICLE, WEIGHT>::
    resampleWithHalfCrossover( const int numberOfSamples, const WEIGHT weight )
// --------------------------------------------------------------------------
{
  // NOTE: This implementation goes back to the CARMEN resampling
  //       method used in localizecore.c:carmen_localize_resample(...)

  register       WEIGHT position2, step_size;
  register const WEIGHT _newSampleWeight = weight;
  register       WEIGHT accumulated_weight = 0;
#ifdef HAVE_LOG_WEIGHTS
  register const WEIGHT _newSampleLogWeight = log( _newSampleWeight );
#endif

  // Set sample pointer to first sample of array...
  register PARTICLE * _pWhichParticle1 = m_particles;
  register PARTICLE * _pWhichParticle2 = m_particles;
  register PARTICLE * _pNewSample     = m_particlesTemp;

  ++m_pfState.numberOfResamplings;
  assert( m_pfState.normalized );

  // Although the sample set is normalized, we have to use the last
  // accumulated weight because of floating point number problems
  // (accumulated weight may be close to, but unequal to 1.0)
  const WEIGHT weight_sum = m_particles[ m_pfState.size-1 ].accumulated_weight;

  // Choose random starting position for low-variance walk
  position2 = carmen_uniform_random( 0, weight_sum );
  step_size = weight_sum / m_pfState.size;

  WEIGHT cx, cy, wf;

  // Draw num_particles random samples
  for ( int i = 0; i < numberOfSamples; i++ )
  {
    // Sample second particle
    position2 += step_size;
    if ( position2 >= weight_sum )
    {
      position2 -= weight_sum;
      _pWhichParticle2 = m_particles;
    }

    while ( position2 > _pWhichParticle2->accumulated_weight )
      ++_pWhichParticle2;

    wf = 1.0 / ( _pWhichParticle1->weight + _pWhichParticle2->weight );
    _pNewSample->x = wf * ( _pWhichParticle1->x * _pWhichParticle1->weight + _pWhichParticle2->x * _pWhichParticle2->weight );
    _pNewSample->y = wf * ( _pWhichParticle1->y * _pWhichParticle1->weight + _pWhichParticle2->y * _pWhichParticle2->weight );

    cx  = cos( _pWhichParticle1->theta ) * _pWhichParticle1->weight;
    cx += cos( _pWhichParticle2->theta ) * _pWhichParticle2->weight;
    cy  = sin( _pWhichParticle1->theta ) * _pWhichParticle1->weight;
    cy += sin( _pWhichParticle2->theta ) * _pWhichParticle2->weight;

    _pNewSample->theta = atan2( cy, cx );

    _pNewSample->weight             = _newSampleWeight;
#ifdef HAVE_LOG_WEIGHTS
    _pNewSample->log_weight         = _newSampleLogWeight;
#endif
    accumulated_weight += _newSampleWeight;
    _pNewSample->accumulated_weight = accumulated_weight;
    ++_pNewSample;

    // First particle is chosen deterministically
    ++_pWhichParticle1;
  }

  // Swap actual and temporary data
  PARTICLE *tmp    = m_particles;
  m_particles      = m_particlesTemp;
  m_particlesTemp  = tmp;

  m_pfState.size = numberOfSamples;

  // Postconditions
  m_pfState.normalized       = true;
  m_pfState.centroidComputed = false;
  m_pfState.varianceComputed = false;

  m_pfState.effectiveSampleSize = m_pfState.size;
  m_pfState.essComputed      = true;
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
void ParticleFilter<PARTICLE, WEIGHT>::
    save()
// --------------------------------------------------------------------------
{
  memcpy( m_particlesBackup, m_particles, m_pfState.maxSize*sizeof(PARTICLE) );
  m_backupPFState = m_pfState;
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
void ParticleFilter<PARTICLE, WEIGHT>::
    restore()
// --------------------------------------------------------------------------
{
  memcpy( m_particles, m_particlesBackup, m_pfState.maxSize*sizeof(PARTICLE) );
  m_pfState = m_backupPFState;
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
int ParticleFilter<PARTICLE, WEIGHT>::
    getParticleRandomly()
// --------------------------------------------------------------------------
{
  // Get random number between 0.0 and 1.0
//   const double random = m_pUniformDistribution->getRandomNumber();
//   const double random = carmen_uniform_random( 0, 1 );

  const WEIGHT weight_sum = m_particles[ m_pfState.size-1 ].accumulated_weight;
  const double random = carmen_uniform_random( 0, weight_sum );

  const int    _size = size();
  register int min = 0, half;
  register int len = _size;

  // Binary search for sample whose accumulated probability fits the uniform random value
  while ( len > 0 )
  {
    half = len / 2;

    if ( m_particles[ min + half ].accumulated_weight > random )
    {
      // step into left half of array
      len = half;
    }
    else
    {
      min +=  half+1;      // step into right half of array
      len -= (half+1);
    }
  }

  if ( min == _size )
    --min;

  return min;
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
uint ParticleFilter<PARTICLE, WEIGHT>::
    getNumberOfResamplings() const
// --------------------------------------------------------------------------
{
  return m_pfState.numberOfResamplings;
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
inline bool ParticleFilter<PARTICLE, WEIGHT>::
    isNormalized() const
// --------------------------------------------------------------------------
{
  return m_pfState.normalized;
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
inline void ParticleFilter<PARTICLE, WEIGHT>::
    setNormalized( const bool isNormalized )
// --------------------------------------------------------------------------
{
  m_pfState.normalized = isNormalized;
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
inline void ParticleFilter<PARTICLE, WEIGHT>::
    resize( const int size )
// --------------------------------------------------------------------------
{
  m_pfState.size = size;
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
inline void ParticleFilter<PARTICLE, WEIGHT>::
    addParticle( const PARTICLE & s )
// --------------------------------------------------------------------------
{
  m_particles[ m_pfState.size ] = s;
  ++m_pfState.size;
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
inline int ParticleFilter<PARTICLE, WEIGHT>::
    size() const
// --------------------------------------------------------------------------
{
  return m_pfState.size;
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
inline int ParticleFilter<PARTICLE, WEIGHT>::
    maxSize() const
// --------------------------------------------------------------------------
{
  return m_pfState.maxSize;
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
inline int ParticleFilter<PARTICLE, WEIGHT>::
    getKLDNumberOfBinsWithSupport() const
// --------------------------------------------------------------------------
{
  return m_iKLDNumberOfBinsWithSupport;
}


// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT >
inline int ParticleFilter<PARTICLE, WEIGHT>::
    getMostLikelyParticle() const
// --------------------------------------------------------------------------
{
  return m_pfState.mostLikelyParticle;
}
