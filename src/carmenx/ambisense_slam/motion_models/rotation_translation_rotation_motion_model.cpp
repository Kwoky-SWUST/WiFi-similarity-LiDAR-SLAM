#include <carmen/carmen.h>
#include <motion_models/rotation_translation_rotation_motion_model.h>
#include <utils/quality/sciroco_exception.h>

using namespace std;


// #define USE_OLD_MOTION_MODEL


// --------------------------------------------------------------------------
RotationTranslationRotationMotionModel::
    RotationTranslationRotationMotionModel( MotionModelParameters* mmParams,
                                            FastGaussRand<double>*  rand)
  : MotionModel( mmParams, "rotation_translation_rotation" )
// --------------------------------------------------------------------------
{
  m_pFastGaussRand = rand;
  ScirocoException::test( m_pFastGaussRand != NULL,
                          "Gaussian random number generator is NULL",
                          __FUNCTION__, __LINE__ );
}


// --------------------------------------------------------------------------
RotationTranslationRotationMotionModel::
    ~RotationTranslationRotationMotionModel()
// --------------------------------------------------------------------------
{
}


// --------------------------------------------------------------------------
void RotationTranslationRotationMotionModel::
    apply( carmen_point_t         odometry,
           double                 timeDifference,
           unsigned int           localizationStep,
           std::vector<Particle>& particles )
// --------------------------------------------------------------------------
{
  double dtheta1, dtheta2;
  double translation = sqrt( odometry.x * odometry.x + odometry.y * odometry.y );

  // Two rotations: one before, one after translation
#ifdef USE_OLD_MOTION_MODEL
  dtheta1 = 0.5 * odometry.theta;
  dtheta2 = 0.5 * odometry.theta;
#else
  // Ensure minimum rotation to compute meaningful angles
  if ( translation >= 0.1 )
  {
    dtheta1 = atan2( odometry.y, odometry.x );
    // NOTE: It is VERY important to check the direction of travel
    //       and flip the first rotation angle if the robot moves backwards
    if ( odometry.x < 0.0 )
    {
      translation *= -1;
      dtheta1 = carmen_normalize_theta( dtheta1 + M_PI );
    }
  }
  else
    dtheta1 = 0.0;

  // The second rotation (after translation) is the difference between the 
  // total rotation and the first rotation. 
  dtheta2 = carmen_normalize_theta( odometry.theta - dtheta1 );
#endif

  // Counter-rotation by first modeled rotation, because the translation
  // will be along the axis after the first rotation
  double _cos = cos( - dtheta1 );
  double _sin = sin( - dtheta1 );
  double dx = odometry.x, dy = odometry.y;
  odometry.x = _cos*dx - _sin*dy;
  odometry.y = _sin*dx + _cos*dy;

  // Apply noise that affects the entire reading, if enabled
  noisify( dtheta1, translation, dtheta2, timeDifference, localizationStep );

  propagateParticles( particles,
                      dtheta1, translation, dtheta2, timeDifference, localizationStep );
}


// --------------------------------------------------------------------------
void RotationTranslationRotationMotionModel::
    propagateParticles( std::vector<Particle>& particles,
                        double dtheta1,
                        double translation,
                        double dtheta2,
                        double /*timeDifference*/,
                        unsigned int localizationStep )
// --------------------------------------------------------------------------
{
  if ( dtheta1 > 10.0/180.0*M_PI )
  {
    cerr << "WARNING: large dtheta1 in RotationTranslationRotationMotionModel: ODO: dtheta1=" << dtheta1 << ",  translation=" << translation << ",  dtheta2=" << dtheta2 << '\n';
  }

  double fr = params->config_localization_noise_rot_discount_factor  * exp( - params->config_localization_noise_rot_discount_rate  * localizationStep);
  double fd = params->config_localization_noise_dist_discount_factor * exp( - params->config_localization_noise_dist_discount_rate * localizationStep);
  double stddev_trans_trans = params->rtrStdDevTranslationPerTranslation + fd;
  double stddev_rot_rot     = params->rtrStdDevRotationPerRotation   + fr;
  double stddev_trans_rot   = params->config_localization_noise_rot_per_dist;
  double mean_trans         = params->rtrTranslationMean;
  double mean_rot           = params->rtrRotationMean;

  double _cos, _sin;
  double dx, dy;

  double noisy_len, theta1, theta2;

  // Iterate over all localization particles
  Particle* particle;
  for ( uint p = 0; p < particles.size(); ++p )
  {
    particle = &( particles[p] );

    theta1    = dtheta1     * ( mean_rot   + stddev_rot_rot * m_pFastGaussRand->getRandomNumber() );
    noisy_len = translation * ( mean_trans + stddev_trans_trans * m_pFastGaussRand->getRandomNumber() );

    _cos = cos( particle->theta_z + theta1 );
    _sin = sin( particle->theta_z + theta1 );

    dx = _cos * noisy_len;
    dy = _sin * noisy_len;

    theta2 = dtheta2 * ( mean_rot + stddev_rot_rot * m_pFastGaussRand->getRandomNumber() ) 
        + noisy_len * stddev_trans_rot * m_pFastGaussRand->getRandomNumber();

    particle->x       += dx;
    particle->y       += dy;
    particle->theta_z = carmen_normalize_theta( particle->theta_z + theta1 + theta2 );
  }
}


// --------------------------------------------------------------------------
void RotationTranslationRotationMotionModel::
    noisify(  double& dtheta1,
              double& translation,
              double& dtheta2,
              double& /*timeDifference*/,
              unsigned int& /*localizationStep*/ )
// --------------------------------------------------------------------------
{
  double abs_rot = fabs( dtheta1 ) + fabs( dtheta2 );

  // Add artificial noise if desired //////////////////////
  if ( params->config_artificial_noise_dist_per_dist > 0.0 )
  {
    translation *= ( 1.0 + m_pFastGaussRand->getRandomNumber() * params->config_artificial_noise_dist_per_dist ); //carmen_gaussian_random( 1.0, config_artificial_noise_dist_per_dist * len );
//     cout << "_________________________ lr=" << lr << endl;
  }

  if ( params->config_artificial_noise_rot_per_dist > 0.0 )
  {
    double r1 = m_pFastGaussRand->getRandomNumber() * params->config_artificial_noise_rot_per_dist * translation; //carmen_gaussian_random( 0.0, config_artificial_noise_rot_per_dist * len );
    double r2 = m_pFastGaussRand->getRandomNumber() * params->config_artificial_noise_rot_per_dist * translation;//carmen_gaussian_random( 0.0, config_artificial_noise_rot_per_dist * len );
//     cout << "_________________________" << r1 << "/" << r2 << endl;
    dtheta1 += r1;
    dtheta2 += r2;
  }

  if ( params->config_artificial_noise_dist_per_rot > 0.0 )
  {
    translation *= 1.0 + m_pFastGaussRand->getRandomNumber() * params->config_artificial_noise_dist_per_rot * abs_rot;
  }

  if ( params->config_artificial_noise_rot_per_rot > 0.0 )
  {
    double r1 = m_pFastGaussRand->getRandomNumber() * params->config_artificial_noise_rot_per_rot * dtheta1; //carmen_gaussian_random( 0.0, config_artificial_noise_rot_per_rot * abs_rot );
    double r2 = m_pFastGaussRand->getRandomNumber() * params->config_artificial_noise_rot_per_rot * dtheta2; //carmen_gaussian_random( 0.0, config_artificial_noise_rot_per_rot * abs_rot );
//     cout << "_________________________" << r1 << "/" << r2 << endl;
    dtheta1 += r1;
    dtheta2 += r2;
  }
  // END of artificial noise //////////////////////////////
}
