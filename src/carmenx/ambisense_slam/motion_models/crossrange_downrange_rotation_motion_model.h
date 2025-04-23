#ifndef SCIROCO_CARMEN__AMBISENSE_SLAM__CROSSRANGE_DOWNRANGE_ROTATION_MOTION_MODEL
#define SCIROCO_CARMEN__AMBISENSE_SLAM__CROSSRANGE_DOWNRANGE_ROTATION_MOTION_MODEL

#include <motion_models/motion_model.h>
#include <ambisense_slam_config.h>


/** Motion model based on the one learned by CARMEN model_learn
 *  (in src/localize/model_learn)
 *  @see (Eliazar/Parr: DP-SLAM)
 *  @author Philipp Vorst
 */
// --------------------------------------------------------------------------
class CrossrangeDownrangeRotationMotionModel : public MotionModel
// --------------------------------------------------------------------------
{
  public:
    /** Constructor
     *  @param mmParams The motion model parameters
     *  @param rand Gaussian random number generator to be reused
     */
    CrossrangeDownrangeRotationMotionModel( MotionModelParameters* mmParams,
                                            FastGaussRand<double>*  rand
                                          );

    /** Destructor */
    virtual ~CrossrangeDownrangeRotationMotionModel();

    /** Apply the given motion to the sample set
     */
    virtual void apply( carmen_point_t         odometry,
                        double                 timeDifference,
                        unsigned int           localizationStep,
                        std::vector<Particle>& particles );

    /** Return the probability of the given transformation, given
     *  the measured odometry reading.
     */
    virtual double getProbability( const carmen_point_t& transformation,
                                   const carmen_point_t& odometry ) const;


    virtual void learnParameters( const std::vector< std::pair< carmen_point_t, carmen_point_t > > motionMeasurements );

  protected:
    /** Add artificial noise, as specified in the motion model parameters */
    void noisify(  double& odo_dx,
                   double& odo_dy,
                   double& dtheta1,
                   double& dtheta2,
                   double& timeDifference,
                   unsigned int& localizationStep );

    /** Propagate the particles according to the given odometry values */
    void propagateParticles( std::vector<Particle>& particles,
                             double translation,
                             double rotation,
                             double timeDifference,
                             unsigned int localizationStep );

    /** Propagate the particles according to the given odometry values */
    void propagateParticlesWRONG( std::vector<Particle>& particles,
                                  double odo_dx,
                                  double odo_dy,
                                  double dtheta1,
                                  double dtheta2,
                                  double timeDifference,
                                  unsigned int localizationStep );

    /** Learn with closed form solution @see "Learning Probabilistic Motion Models for Mobile Robots" A. Eliazar, R. Parr; 2004 (carmen/src/localize/model_learn/learn.c)
     * @param motionMeasurements vector of pairs of < odometry, ground_truth > samples used for learning
     */
    void learnParametersClosedFormLeastSquares( const std::vector< std::pair< carmen_point_t, carmen_point_t > > motionMeasurements );
    /** Helper for closed form least squares */
    void leastSquaresClosedForm( const double sumNormal[2][2], const double sumY[2], double leastSquaresSolution[2] );

  protected:
    /// The Gaussian random number generator supplied to the particle filters
    FastGaussRand<double>*  m_pFastGaussRand;

};


#endif // SCIROCO_CARMEN__AMBISENSE_SLAM__CROSSRANGE_DOWNRANGE_ROTATION_MOTION_MODEL
