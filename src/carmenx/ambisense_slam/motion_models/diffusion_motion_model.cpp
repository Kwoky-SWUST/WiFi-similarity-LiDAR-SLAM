#include <carmen/carmen.h>
#include <motion_models/diffusion_motion_model.h>
#include <utils/quality/sciroco_exception.h>

using namespace std;


// --------------------------------------------------------------------------
DiffusionMotionModel::
    DiffusionMotionModel( MotionModelParameters* mmParams,
                          FastGaussRand<double>*  rand)
  : MotionModel( mmParams, "diffusion" )
// --------------------------------------------------------------------------
{
  m_pFastGaussRand = rand;
  ScirocoException::test( m_pFastGaussRand != NULL,
                          "Gaussian random number generator is NULL",
                          __FUNCTION__, __LINE__ );
}


// --------------------------------------------------------------------------
DiffusionMotionModel::
    ~DiffusionMotionModel()
// --------------------------------------------------------------------------
{
}


// --------------------------------------------------------------------------
void DiffusionMotionModel::
    apply( carmen_point_t         odometry,
           double                 timeDifference,
           unsigned int           /*localizationStep*/,
           std::vector<Particle>& particles )
// --------------------------------------------------------------------------
{
  VERBOSE_OUT << color::purple << "Odometry GAUSSIAN_DIFFUSION: x=" << odometry.x << ",  y=" << odometry.y << ", theta=" << odometry.theta << color::normal << endl;

  // Iterate over all localization particles
  for ( uint p = 0; p < particles.size(); ++p )
  {
    Particle& particle = particles[p];

    particle.x       += m_pFastGaussRand->getRandomNumber() * params->rtrStdDevTranslationPerTranslation * timeDifference;
    particle.y       += m_pFastGaussRand->getRandomNumber() * params->rtrStdDevTranslationPerTranslation * timeDifference;
    particle.theta_z = carmen_normalize_theta( particle.theta_z + m_pFastGaussRand->getRandomNumber() * params->rtrStdDevRotationPerRotation * timeDifference );
  }
}
