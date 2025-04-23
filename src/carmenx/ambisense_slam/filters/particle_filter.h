#ifndef AMBISENSE_SLAM__PARTICLE_FILTER_H
#define AMBISENSE_SLAM__PARTICLE_FILTER_H

#include "mcl_random/fast_gauss_rand.h"
#include "mcl_random/fast_uniform_rand.h"


/** Implementation of sample management */
// --------------------------------------------------------------------------
template< class PARTICLE, class WEIGHT=double >
class ParticleFilter
// --------------------------------------------------------------------------
{
public:
  typedef double (*LikelihoodFunction)(PARTICLE*, void*);

  /// Statistics about the current state of the particle filter
  struct ParticleFilterState
  {
    /// Actual size of the particle filter
    int  size;

    /// Maximal size of the particle filter
    int  maxSize;

    /// Has the particle filter been normalized since the last reset?
    bool normalized;

    /// Has the centroid been computed since the last reset?
    bool centroidComputed;

    /// Has the variance been computed since the last reset?
    bool varianceComputed;

    /// The effective particle filter size
    /// @see getEffectiveSampleSize, m_bESSComputed
    uint effectiveSampleSize;

    /// Has the effective particle filter size been computed since the last reset?
    /// @see getEffectiveParticleSetSize
    bool essComputed;

    /// The index of the most likely particle within the array of particles
    int  mostLikelyParticle;

    /// The centroid defined by the particles
    PARTICLE   center;

    /// The variance among the particles
    PARTICLE   variance;

    /// The x-y covariance among the particles
    WEIGHT     xyCovariance;

    /// The recent accumulated belief for all particles in this set
    WEIGHT sumOfWeights;

    /// The maximum of all beliefs of the particles in this set
    WEIGHT maxWeight;

    /// The minimum of all beliefs of the particles in this set
    WEIGHT minWeight;

    /// Resampling counter
    uint numberOfResamplings;
  };


public:
  /** Construct a particle filter which can capture the given number of particles. */
  ParticleFilter<PARTICLE, WEIGHT>( const int size,
                                    const FastGaussRand< WEIGHT >* generator,
                                    const char *name = "n/a" );

  /** Construct a particle filter from the given source particle filter */
  ParticleFilter<PARTICLE, WEIGHT>( ParticleFilter<PARTICLE, WEIGHT> & source,
                                    const FastGaussRand< WEIGHT > * generator );

  /** Destructor */
  ~ParticleFilter<PARTICLE, WEIGHT>();

  /** Reset the particle filter, i.e. the filter knows that it has not been
   *  normalized, yet, for instance. */
  void reset();

  /** Reset the precomputed value of the PF center and variance */
  void resetCenterAndVariance();

  /** Copy all particles from the given particle filter. This will override
   *  the current samples of this particle filter.
   *   @param src The particle filter to copy the particles from.
   */
  void copyFrom( const ParticleFilter<PARTICLE, WEIGHT> * src );

  /** Return number of particles in particle filter. */
  inline int size() const;

  /** Return the maximal number of particles in particle filter. */
  inline int maxSize() const;

  /** Set the number of particles in particle filter. */
  inline void resize( const int size );

  /** Insert sample to particle filter
   *  @note No referencing!, always 'const PARTICLE s'
   */
  inline void addParticle( const PARTICLE & s );

  /** Perform KD-sampling on the particle filter. */
  void kldSample( LikelihoodFunction fct, void* sensorData );

  /** Prepapre KD sampling. Has to be called once in the beginning,
   *  before the first call to kdSample.
   */
  void kldInitialize( double mapMinX,  double mapMaxX,
                      double mapMinY,  double mapMaxY,
                      double binWidth, double binAngle,
                      int    minimalNumberOfParticles,
                      double delta = 0.99, double epsilon = 0.05
                    );

  /** Get the number of bins with support.
   *  @see m_iKLDNumberOfBinsWithSupport
   */
  inline int getKLDNumberOfBinsWithSupport() const;

  /** Re-sample particle filter. Precondition: The particle filter has been normalized. */
  void resample( const int numberOfOldParticles, const WEIGHT weight );

  /** Re-sample specific fraction of current particle filter. */
  void resampleFraction( const double fractionOfOldParticles )
          { resample( (int) ( fractionOfOldParticles*size(), 1.0/size() ) ); }

  /** Resample the entire sample set */
  void resample()
          { resample( size(), 1.0/size() ); }

  /** Resample the particles by crossing them over */
  void resampleWithCrossover( const int numberOfSamples,
                              const WEIGHT weight );

  /** Resample the entire sample set by crossing particles over */
  void resampleWithCrossover()
          { resampleWithCrossover( size(), 1.0/size() ); }

  /** Resample the particles by crossing them over. One particle is chosen
   ** deterministically (the old particle i), the other one is sampled
   ** proportionally to its weight.
   **/
  void resampleWithHalfCrossover( const int numberOfSamples,
                              const WEIGHT weight );

  /** Resample the entire particle set incl. cross-over. One particle is chosen
   ** deterministically (the old particle i), the other one is sampled
   ** proportionally to its weight.
   **/
  void resampleWithHalfCrossover()
          { resampleWithHalfCrossover( size(), 1.0/size() ); }

  /** Normalize particle filter, i.e. make sum of probs accumulate to 1. */
  void normalize();

  /** Normalize particle filter, i.e. make sum of probs accumulate to 1. */
  void normalizeWithLogWeights();

  /** Save a copy of the current particle filter state/ its sample set
   *  @see restore
   */
  void save();

  /** Roll back the recent changes. The particle filter will use the previously
   *  saved sample set.
   *  @see save
   */
  void restore();

  /** Add Gaussian noise to all particles */
  void addGaussianNoise( const WEIGHT translationStdDev,
                         const WEIGHT orientationStdDev = 0 );

  /** Add Gaussian noise to all particles */
  void initializeGaussian( const PARTICLE & defaultPosition,
                           const WEIGHT translationStdDev,
                           const WEIGHT orientationStdDev = 0 );

  /** Get the number of times that the particle set has been resampled */
  uint getNumberOfResamplings() const;

  /** Draw a sample randomly from particle filter (according to its
   *  weight) and return its index. */
  int getParticleRandomly();

  /** Returns i-th sample. Note: risky access!! */
  PARTICLE & operator [] ( const int i )
          { 	return m_particles[i]; }

  PARTICLE *at( int i ) { return &m_particles[i]; }

  /** Return the number of particles which effectively contribute to the
   *  probability distribution. This is a measure of the degeneracy of the
   *  particle filter. */
  int getEffectiveSampleSize();

  /// Return the recent accumulated belief for all particles in the particle filter
  WEIGHT getStatisticsEta()       { return m_pfState.sumOfWeights; }

  /// Return the maximum of all beliefs of the particles in this set
  WEIGHT getStatisticsMaxWeight() { return m_pfState.maxWeight; }

  /// Return the minimum by the maximum of all sample probabilities
  WEIGHT getStatisticsMinWeight() { return m_pfState.minWeight; }

  /// Remove all particles from the current particle filter
  void clear();

  /** Get the name (user-defined identifier) of this particle filter */
  const char* getName() const { return m_name; }

  /** Get the centroid of the particle filter */
  PARTICLE & getCenter();

  /** Get the variance among the particle filter */
  PARTICLE & getVariance();

  /** Get the x-y covariance among the particle filter.
   *  @note This method requires that the variance has been computed before! This
   *        is done by calling getVariance()
   *  @see  getVariance
   */
  inline WEIGHT getXYCovariance();

  /** Return the raw data array. */
  inline const PARTICLE * getParticles() const;

  /** Is the sample set normalized? */
  inline bool isNormalized() const;

  /** Set the sample set (un)normalized */
  inline void setNormalized( const bool isNormalized );

  /** Get the index of the most likely particle */
  inline int getMostLikelyParticle() const;


public:
  // Public variables for faster access

  /// The array which contains the particles
  PARTICLE *m_particles;

  /// The array which is used temporarily in computations, e.g. for resampling, normalization etc.
  PARTICLE *m_particlesTemp;

  /// The array which is used for sample set backups
  /// @see save, restore
  PARTICLE *m_particlesBackup;


protected:
  /// Uniform random number generator
//   FastUniformRand< WEIGHT > * m_pUniformDistribution;

  /// Gaussian random number generator
  FastGaussRand< WEIGHT >   * m_pGaussGenerator;

  /// The name of this particle filter
  char * m_name;

  /// Statistics about the current state of the particle filter
  ParticleFilterState  m_pfState;

  /// Statistics about the current state of the backup copy of
  /// the former particle filter
  ParticleFilterState  m_backupPFState;


// KLD sampling parameters ////////////////////////////////
  /// Minimal number of particles
  int    m_iKLDMinNumberOfParticles;

  /// Width of bins in KD sampling
  double m_dKLDSamplingBinWidth;

  /// Angle tolerance for bins in KD sampling
  double m_dKLDSamplingBinAngle;

  double m_dKLDSamplingMinX;
  double m_dKLDSamplingMinY;

  int    m_iKLDSamplingMapWidth;
  int    m_iKLDSamplingMapHeight;
  int    m_iKLDSamplingOrientations;

  double m_dKLDSamplingDelta;
  double m_dKLDEpsilon;

  /** Upper (1-m_dKLDSamplingDelta) quantile of the standard normal
   *  N(0,1) distribution. Precomputed in kldInitialize
   *  @see kldInitialize
   */
  double m_dKLDSamplingUpperQuantile;

  /** Recent number of bins with support */
  int    m_iKLDNumberOfBinsWithSupport;

  /** Array of flags which state whether or not a bin has support.
   *  @todo A tree would represent a more sophisticated data
   *        structure for this.
   */
  std::vector< bool > m_vKLDSamplingBins;

  /** Precomputed KL bounds */
  int * m_iKLBounds;

};

#include "particle_filter.cpp"

#endif // AMBISENSE_SLAM__PARTICLE_FILTER_H
