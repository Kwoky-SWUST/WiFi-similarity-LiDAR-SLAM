#ifndef AMBISENSE_SLAM__FILTERS__PF2_XYTHETA_H
#define AMBISENSE_SLAM__FILTERS__PF2_XYTHETA_H

#include <carmenx/ambisense_slam/ambisense_slam_messages.h>
#include "particle_filter_2.h"
#include <utils/geometry/pose.h>
#include <mcl_random/fast_gauss_rand.h>

#ifndef SCIROCO_CARMEN__AMBISENSE_SLAM__AMBISENSE_SLAM_CONFIG_H
#error You MUST include ambisense_slam_config.h first. 
#endif


namespace pf2
{
  /** Specialized version of a particle for
   *  6D poses (3D position + 3D orientation)
   */
  class ParticleXYTheta : public carmen_point_t
  {
    public:
/*      ParticleXYTheta(); ///< Constructor
      ParticleXYTheta( const ParticleXYTheta& p ); ///< Constructor
      ParticleXYTheta( const carmen_6d_point_t& p ); ///< Create a particle with the given 3D pose
      ParticleXYTheta( const carmen_point_t& p ); ///< Constructor
//       virtual ~ParticleXYTheta();
      void setPosition( const carmen_6d_point_t& p ); ///< Set the particle to the given 3D pose
      void setPosition( const carmen_point_t&    p ); ///< Set the particle to the given 2D pose
      carmen_ambisense_particle_ipc_t asIPCParticle() const; ///< Turn the particle into an IPC-publishable type
*/
      /// Assignment operator is overloaded because assignments are 
      /// significantly speeded up!
      ParticleXYTheta& operator = ( const ParticleXYTheta& p )
      {
#ifdef HAVE_PARTICLE_TRAJECTORY
        if ( p.trajectory.size() > 0 )
        {
          trajectory.resize( p.trajectory.size() );
          copy( p.trajectory.begin(), p.trajectory.end(), trajectory.begin() );
        }
#endif

        x                  = p.x;
        y                  = p.y;
        theta              = p.theta;
        weight             = p.weight;
  #ifdef HAVE_LOG_WEIGHTS
        log_weight         = p.log_weight;
  #endif
        return *this;
      }

    public:
      /// Importance weight of the particle
      double weight;

#ifdef HAVE_LOG_WEIGHTS
      /// Logarithm of the importance weight of the particle
      double log_weight;
#endif

#ifdef HAVE_PARTICLE_TRAJECTORY
      /// History of past poses
      std::vector< carmen_point_t > trajectory;
#endif
  };

  /** Specialized version of a particle filter
   *  with 6D poses
   */
  class ParticleFilterXYTheta : public pf2::ParticleFilter< pf2::ParticleXYTheta >
  {
    public:
      ParticleFilterXYTheta();
      void computeMeanAndCovariance( Pose& mean, double cov[36] );
      void computeMeanAndVariance2D( Pose& mean, Pose& variance, double& xy_covariance );
      void computeMean( Pose& mean );

      /** Add Gaussian noise to all particles */
      void addGaussianNoise( FastGaussRand< double >* generator,
                             const double translationStdDev,
                             const double orientationStdDev = 0 );

      friend std::ostream & operator << ( std::ostream& ostr, const ParticleFilterXYTheta& pf );
  };
}

#endif // AMBISENSE_SLAM__FILTERS__PF2_XYTHETA_H
