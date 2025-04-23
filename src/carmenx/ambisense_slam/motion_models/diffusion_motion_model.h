#ifndef SCIROCO_CARMEN__AMBISENSE_SLAM__DIFFUSION_MOTION_MODEL
#define SCIROCO_CARMEN__AMBISENSE_SLAM__DIFFUSION_MOTION_MODEL

#include <motion_models/motion_model.h>


/** Motion model based on Gaussian diffusion
 *  @author Philipp Vorst
 */
// --------------------------------------------------------------------------
class DiffusionMotionModel : public MotionModel
// --------------------------------------------------------------------------
{
  public:
    /** Constructor */
    DiffusionMotionModel( MotionModelParameters* mmParams,
                          FastGaussRand<double>*  rand );

    /** Destructor */
    virtual ~DiffusionMotionModel();

    /** Apply the given motion to the sample set 
     */
    virtual void apply( carmen_point_t         odometry,
                        double                 timeDifference,
                        unsigned int           localizationStep,
                        std::vector<Particle>& particles );

  protected:
    /// The Gaussian random number generator supplied to the particle filters
    FastGaussRand<double>*  m_pFastGaussRand;

};


#endif // SCIROCO_CARMEN__AMBISENSE_SLAM__DIFFUSION_MOTION_MODEL
