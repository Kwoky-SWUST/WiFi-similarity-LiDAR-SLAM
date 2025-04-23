#ifndef AMBISENSE_SLAM__FILTERS__PF2_HISTORY_6D_H
#define AMBISENSE_SLAM__FILTERS__PF2_HISTORY_6D_H

#include <filters/pf2_6d.h>
#include <filters/particle_filter_history.h>
#include <vector>


namespace pf2
{
  /** Specialized version of a particle filter history for
   *  6D poses (3D position + 3D orientation)
   *  @author Philipp Vorst
   */
  // ------------------------------------------------------------------------
  class ParticleFilterHistory6D 
    : public pf2::ParticleFilterHistory< pf2::Particle6D >
  // ------------------------------------------------------------------------
  {
    public:
      enum SmoothingMethod {
        SIMPLE_WEIGHTING_SMOOTHING, ///< Smooth by weighted averaging of the trajectories, using the latest sample weights
        BACKWARD_SMOOTHING,         ///< Forward filtering, backward smoothing
        MOST_LIKELY_PARTICLE        ///< Rather than smoothing, use the path of the most likely particle
      };

    public:
      /** Constructor */
      ParticleFilterHistory6D() : pf2::ParticleFilterHistory< pf2::Particle6D >() {};

      /** Compute a smoothed trajectory, based on the particle filter
       *  history.
       */
      virtual ParticleHistory smooth( const MotionModel* m, SmoothingMethod method = MOST_LIKELY_PARTICLE );

      /** Return the 2D trajectory of given index */
      std::vector< carmen_point_t > get2DTrajectory( uint index ) const;

      void smoothBackward( const MotionModel* /*m*/,
                           const std::vector< carmen_point_t >& odometry 
                         );

    protected:
      double getTransitionProbability( const MotionModel* motionModel,
                                       const carmen_point_t& transition, 
                                       const carmen_point_t& odo );

    protected:
      /// Rather than smoothing, use the path of the most likely particle
      /// @see MOST_LIKELY_PARTICLE
      ParticleHistory smoothingByMostLikelyParticle();

      /// Smooth by weighted averaging of the trajectories, using the latest sample weights
      /// @see SIMPLE_WEIGHTING_SMOOTHING
      ParticleHistory smoothingBySimpleWeighting();

      /// Forward filtering, backward smoothing
      /// @see BACKWARD_SMOOTHING
      ParticleHistory smoothingByBackwardSmoothing( const MotionModel* m );
  };
}

#endif // AMBISENSE_SLAM__FILTERS__PF2_HISTORY_6D_H
