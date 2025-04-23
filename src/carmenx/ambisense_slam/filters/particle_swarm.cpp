#include <carmen/carmen.h>
#include "particle_swarm.h"

using namespace std;


// --------------------------------------------------------------------------
pf2::PSO::
    PSO()
  : pf2::ParticleFilter< SwarmParticle >()
// --------------------------------------------------------------------------
{
}


// --------------------------------------------------------------------------
void pf2::PSO::
    computeMeanAndVariance2D( carmen_point_t & mean,
                              carmen_point_t & variance,
                              double & xy_covariance )
// --------------------------------------------------------------------------
{
  mean = variance = (carmen_point_t) { 0,0,0 };
  xy_covariance = 0.0;
  double theta_x = 0.0, theta_y = 0.0;
  if ( size() == 0 )
    return;

  for ( SwarmConstIterator it = particles.begin(); it != particles.end(); ++it )
  {
    mean.x  += it->pose.x * it->weight;
    mean.y  += it->pose.y * it->weight;
    theta_x += cos( it->pose.theta_z ) * it->weight;
    theta_y += sin( it->pose.theta_z ) * it->weight;
  }
  mean.theta = atan2( theta_y, theta_x );
}


// --------------------------------------------------------------------------
void pf2::PSO::
    computeCurrentBestPosition( carmen_6d_point_t & best )
// --------------------------------------------------------------------------
{
  int mlpi = getMostLikelyParticle();
  if ( mlpi < 0 )
  {
    best = (carmen_6d_point_t) { 0,0,0,0,0,0 };
  }
  else
  {
    best = particles[ mlpi ].pose;
  }
}


// --------------------------------------------------------------------------
std::ostream & operator << ( std::ostream & ostr, const pf2::PSO & pso )
// --------------------------------------------------------------------------
{
  for ( pf2::SwarmConstIterator it = pso.particles.begin(); it != pso.particles.end(); ++it )
  {
    ostr << "(" << it->pose.x << ", " << it->pose.y << "), vel. (" << it->velocity.x << ", " << it->velocity.y << ")\n";
  }
  return ostr;
}
