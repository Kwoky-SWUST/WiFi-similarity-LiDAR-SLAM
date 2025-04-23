#ifndef SCIROCO_CARMEN__AMBISENSE_SLAM__OLD_CROSSRANGE_DOWNRANGE_ROTATION_MOTION_MODEL
#define SCIROCO_CARMEN__AMBISENSE_SLAM__OLD_CROSSRANGE_DOWNRANGE_ROTATION_MOTION_MODEL

#include <motion_models/motion_model.h>
#include <ambisense_slam_config.h>


/** Motion model based on the one in Thrun, "Probabilistic Robotics" (2006), Ch. 5.4.
 *  @author Philipp Vorst
 */
// --------------------------------------------------------------------------
class RotationTranslationRotationMotionModel : public MotionModel
// --------------------------------------------------------------------------
{
  public:
    /** Constructor
     *  @param mmParams The motion model parameters
     *  @param rand Gaussian random number generator to be reused
     */
    RotationTranslationRotationMotionModel( MotionModelParameters* mmParams,
                                            FastGaussRand<double>*  rand
                                          );

    /** Destructor */
    virtual ~RotationTranslationRotationMotionModel();

    /** Apply the given motion to the sample set 
     */
    virtual void apply( carmen_point_t         odometry,
                        double                 timeDifference,
                        unsigned int           localizationStep,
                        std::vector<Particle>& particles );

  protected:
    /** Add artificial noise, as specified in the motion model parameters */
    void noisify(  double& dtheta1,
                   double& translation,
                   double& dtheta2,
                   double& timeDifference,
                   unsigned int& localizationStep );

    /** Propagate the particles according to the given odometry values */
    void propagateParticles( std::vector<Particle>& particles,
                             double dtheta1,
                             double translation,
                             double dtheta2,
                             double timeDifference,
                             unsigned int localizationStep );

  protected:
    /// The Gaussian random number generator supplied to the particle filters
    FastGaussRand<double>*        m_pFastGaussRand;

};


#endif // SCIROCO_CARMEN__AMBISENSE_SLAM__OLD_CROSSRANGE_DOWNRANGE_ROTATION_MOTION_MODEL
