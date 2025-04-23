#ifndef SCIROCO_CARMEN__AMBISENSE_SLAM__RIGID_BODY_TRANSFORMATION_MOTION_MODEL
#define SCIROCO_CARMEN__AMBISENSE_SLAM__RIGID_BODY_TRANSFORMATION_MOTION_MODEL

#include <motion_models/motion_model.h>
#include <ambisense_slam_config.h>


/** Motion model based on rigid-body transformation between two poses
 *  @author Philipp Vorst
 */
// --------------------------------------------------------------------------
class RigidBodyTransformationMotionModel : public MotionModel
// --------------------------------------------------------------------------
{
  public:
    /** Constructor
     *  @param mmParams The motion model parameters
      *  @param rand Gaussian random number generator to be reused
     */
    RigidBodyTransformationMotionModel( MotionModelParameters* mmParams,
                                        FastGaussRand<double>*  rand
                                      );

    /** Destructor */
    virtual ~RigidBodyTransformationMotionModel();

    /** Apply the given motion to the sample set 
     */
    virtual void apply( carmen_point_t         odometry,
                        double                 timeDifference,
                        unsigned int           localizationStep,
                        std::vector<Particle>& particles );


  protected:
    /** Add artificial noise, as specified in the motion model parameters */
    void noisify(  carmen_point_t& odometry,
                   const double& timeDifference,
                   const unsigned int& localizationStep ) const;

    /** Propagate the particles according to the given odometry values */
    void propagateParticles( std::vector<Particle>& particles,
                             const carmen_point_t& odometry,
                             const double timeDifference,
                             const unsigned int localizationStep );

  protected:
    /// The Gaussian random number generator supplied to the particle filters
    FastGaussRand<double>*  m_pFastGaussRand;

};


#endif // SCIROCO_CARMEN__AMBISENSE_SLAM__RIGID_BODY_TRANSFORMATION_MOTION_MODEL
