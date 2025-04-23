#ifndef SCIROCO_CARMEN__AMBISENSE_SLAM__NOISELESS_MOTION_MODEL
#define SCIROCO_CARMEN__AMBISENSE_SLAM__NOISELESS_MOTION_MODEL

#include <motion_models/motion_model.h>
#include <ambisense_slam_config.h>


/** Motion model based on rigid-body transformation between two poses
 *  @author Philipp Vorst
 */
// --------------------------------------------------------------------------
class NoiselessMotionModel : public MotionModel
// --------------------------------------------------------------------------
{
  public:
    /** Constructor
      *  @param mmParams The motion model parameters
      */
    NoiselessMotionModel( MotionModelParameters* mmParams );

    /** Destructor */
    virtual ~NoiselessMotionModel();

    /** Apply the given motion to the sample set 
     */
    virtual void apply( carmen_point_t         odometry,
                        double                 timeDifference,
                        unsigned int           localizationStep,
                        std::vector<Particle>& particles );


  protected:
    /** Propagate the particles according to the given odometry values */
    void propagateParticles( std::vector<Particle>& particles,
                             const carmen_point_t& odometry,
                             const double timeDifference,
                             const unsigned int localizationStep );

  protected:

};


#endif // SCIROCO_CARMEN__AMBISENSE_SLAM__NOISELESS_MOTION_MODEL
