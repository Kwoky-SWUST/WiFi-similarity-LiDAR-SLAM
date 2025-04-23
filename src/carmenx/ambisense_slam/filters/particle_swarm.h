#ifndef AMBISENSE_SLAM__FILTERS__PARTICLE_SWARM_H
#define AMBISENSE_SLAM__FILTERS__PARTICLE_SWARM_H

#include "particle_filter_2.h"
#include <ambisense_slam_config.h>

namespace pf2
{
  class SwarmParticle
  {
    public:
      SwarmParticle()
        {
#ifdef HAVE_PARTICLE_TRAJECTORY
          trajectory.clear();
#endif
        }
      carmen_6d_point_t  pose;
      carmen_6d_point_t  velocity;
      double             weight;
      int                parent;

      carmen_6d_point_t  personal_best_pose;
      double             personal_best_weight;

#ifdef HAVE_LOG_WEIGHTS
      /// Logarithm of the importance weight of the particle
      double log_weight;
#endif

#ifdef HAVE_PARTICLE_TRAJECTORY
      std::vector< carmen_6d_point_t > trajectory;
#endif
  };

  typedef std::vector< SwarmParticle >                 SwarmParticles;
  typedef std::vector< SwarmParticle >::const_iterator SwarmConstIterator;
  typedef std::vector< SwarmParticle >::iterator       SwarmIterator;

  class PSO : public pf2::ParticleFilter< SwarmParticle >
  {
    public:
      PSO();
      void computeMeanAndVariance2D( carmen_point_t & mean, carmen_point_t & variance, double & xy_covariance );
      void computeCurrentBestPosition( carmen_6d_point_t & best );
      friend std::ostream & operator << ( std::ostream & ostr, const PSO & pso );
  };
}

#endif // AMBISENSE_SLAM__FILTERS__PARTICLE_SWARM_H
