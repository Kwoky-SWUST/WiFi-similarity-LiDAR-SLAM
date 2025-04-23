#include <carmen/carmen.h>
#include <motion_models/rigid_body_transformation_motion_model.h>
#include <utils/quality/sciroco_exception.h>

using namespace std;


// --------------------------------------------------------------------------
RigidBodyTransformationMotionModel::
    RigidBodyTransformationMotionModel( MotionModelParameters* mmParams,
                                        FastGaussRand<double>*  rand)
  : MotionModel( mmParams, "rigid_body_transformation" )
// --------------------------------------------------------------------------
{
  m_pFastGaussRand = rand;
  ScirocoException::test( m_pFastGaussRand != NULL,
                          "Gaussian random number generator is NULL",
                          __FUNCTION__, __LINE__ );
}


// --------------------------------------------------------------------------
RigidBodyTransformationMotionModel::
    ~RigidBodyTransformationMotionModel()
// --------------------------------------------------------------------------
{
}


// --------------------------------------------------------------------------
void RigidBodyTransformationMotionModel::
    apply( carmen_point_t         odometry,
           double                 timeDifference,
           unsigned int           localizationStep,
           std::vector<Particle>& particles )
// --------------------------------------------------------------------------
{
  noisify( odometry, timeDifference, localizationStep );
  propagateParticles( particles,
                      odometry, timeDifference, localizationStep );
}


// --------------------------------------------------------------------------
void RigidBodyTransformationMotionModel::
    noisify(  carmen_point_t& odometry,
              const double& /*timeDifference*/,
              const unsigned int& /*localizationStep*/ ) const
// --------------------------------------------------------------------------
{
  double len     = sqrt( odometry.x*odometry.x + odometry.y*odometry.y );
  double abs_rot = fabs( odometry.theta );

  // Add artificial noise if desired //////////////////////
  if ( params->config_artificial_noise_dist_per_dist > 0.0 )
  {
    double lr = 1.0 + m_pFastGaussRand->getRandomNumber() * params->config_artificial_noise_dist_per_dist * len; 
    odometry.x *= lr;
    odometry.y *= lr;
  }
  if ( params->config_artificial_noise_rot_per_dist > 0.0 )
  {
    double r1 = m_pFastGaussRand->getRandomNumber() * params->config_artificial_noise_rot_per_dist * len; 
    odometry.theta += r1;
  }
  if ( params->config_artificial_noise_dist_per_rot > 0.0 )
  {
    double lr = 1.0 + m_pFastGaussRand->getRandomNumber() * params->config_artificial_noise_dist_per_rot * abs_rot;
    odometry.x *= lr;
    odometry.y *= lr;
  }
  if ( params->config_artificial_noise_rot_per_rot > 0.0 )
  {
    double r1 = m_pFastGaussRand->getRandomNumber() * params->config_artificial_noise_rot_per_rot * abs_rot;
    odometry.theta += r1;
  }
  // END of artificial noise //////////////////////////////
}


// --------------------------------------------------------------------------
void RigidBodyTransformationMotionModel::
    propagateParticles( std::vector<Particle>& particles,
                        const carmen_point_t& odometry,
                        const double /*timeDifference*/,
                        const unsigned int localizationStep )
// --------------------------------------------------------------------------
{
  VERBOSE_OUT << color::purple << "Odometry CROSSRANGE_DOWNRANGE_ROTATION: x=" << odometry.x << ",  y=" << odometry.y << ", theta=" << odometry.theta << color::normal << endl;

  // Special noise for first, global localization steps
  double fr = params->config_localization_noise_rot_discount_factor  * exp( - params->config_localization_noise_rot_discount_rate  * localizationStep );
  double fd = params->config_localization_noise_dist_discount_factor * exp( - params->config_localization_noise_dist_discount_rate * localizationStep );

  bool extra_noise = false;
  if ( fr > 0 || fd > 0 )
    extra_noise = true;

  double  t[3]; // the transformation, relative to frame t
  const double* odo_array = &( odometry.x ); // cast odometry struct to double array
  int     d, d2; // loop variables
  double _cos, _sin, dx, dy;

  // Iterate over all localization particles
  for ( uint p = 0; p < particles.size(); ++p )
  {
    Particle& particle = particles[p];

//     if ( p == 1 )
//     {
//       cout << "debug: propagateParticles: odo_array: " << odo_array[0] << " " << odo_array[1] << " " << odo_array[2] << endl;
//     }

    for ( d = 0; d < 3; ++d )
    {
      t[d] = 0.0;
//       t[d] = odo_array[d] * params->rigidMean[d];

      // Add correlated uncertainties
      for ( d2 = 0; d2 < 3; ++d2 )
      {
        t[d] += odo_array[ d2 ] * m_pFastGaussRand->getRandomNumber() * params->rigidStdDev[d][d2];
      }
    }

    if ( extra_noise )
    {
      t[0] += m_pFastGaussRand->getRandomNumber() * fd;
      t[2] += m_pFastGaussRand->getRandomNumber() * fr;
    }

//     if ( p == 1 )
//     {
//       cout << "debug: propagateParticles: added odo: " << t[0] << " " << t[1] << " " << t[2] << endl;
//     }

    _cos = cos( particle.theta_z );
    _sin = sin( particle.theta_z );

    dx = _cos * t[0] - _sin * t[1];
    dy = _sin * t[0] + _cos * t[1];

    particle.x       += dx;
    particle.y       += dy;
    particle.theta_z = carmen_normalize_theta( particle.theta_z + t[2] );
  }
}
