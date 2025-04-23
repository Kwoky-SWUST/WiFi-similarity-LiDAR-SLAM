#include <carmen/carmen.h>
#include <motion_models/noiseless_motion_model.h>
#include <utils/quality/sciroco_exception.h>

using namespace std;


// --------------------------------------------------------------------------
NoiselessMotionModel::
    NoiselessMotionModel( MotionModelParameters* mmParams )
  : MotionModel( mmParams, "noiseless" )
// --------------------------------------------------------------------------
{
}


// --------------------------------------------------------------------------
NoiselessMotionModel::
    ~NoiselessMotionModel()
// --------------------------------------------------------------------------
{
}


// --------------------------------------------------------------------------
void NoiselessMotionModel::
    apply( carmen_point_t         odometry,
           double                 timeDifference,
           unsigned int           localizationStep,
           std::vector<Particle>& particles )
// --------------------------------------------------------------------------
{
  propagateParticles( particles,
                      odometry, timeDifference, localizationStep );
}


// --------------------------------------------------------------------------
void NoiselessMotionModel::
    propagateParticles( std::vector<Particle>& particles,
                        const carmen_point_t& odometry,
                        const double /*timeDifference*/,
                        const unsigned int /*localizationStep*/ )
// --------------------------------------------------------------------------
{
  cout << "debug: NoiselessMotionModel::propagateParticles" << endl;

  double _cos, _sin, dx, dy;

  // Iterate over all localization particles
  for ( uint p = 0; p < particles.size(); ++p )
  {
    Particle& particle = particles[p];

    _cos = cos( particle.theta_z );
    _sin = sin( particle.theta_z );

    dx = _cos * odometry.x - _sin * odometry.y;
    dy = _sin * odometry.x + _cos * odometry.y;

    particle.x       += dx;
    particle.y       += dy;
    particle.theta_z = carmen_normalize_theta( particle.theta_z + odometry.theta );
  }
}
