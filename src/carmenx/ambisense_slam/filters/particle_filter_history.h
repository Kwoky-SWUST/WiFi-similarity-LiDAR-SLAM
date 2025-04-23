#ifndef AMBISENSE_SLAM__FILTERS__PARTICLE_FILTER_HISTORY_H
#define AMBISENSE_SLAM__FILTERS__PARTICLE_FILTER_HISTORY_H

#include <vector>
//
class MotionModel;


namespace pf2
{

  /** Particle filter history.
   *  PARTICLE_TYPE can be any data structure containing a double-valued
   *  field 'weight'.
   *  @author Philipp Vorst
   */
  // ------------------------------------------------------------------------
  template< class PARTICLE_TYPE >
  class ParticleFilterHistory
  // ------------------------------------------------------------------------
  {
    public:
      /// History of particle states over time
      typedef std::vector< PARTICLE_TYPE > ParticleHistory;

    public:
      /** Constructor */
      ParticleFilterHistory< PARTICLE_TYPE >();

      /** Destructor */
      virtual ~ParticleFilterHistory< PARTICLE_TYPE >();

      /** Add the current filter to the filter history
       *  @param src The particle filter to copy the particles from.
       */
      void addTimeStep( ParticleFilter< PARTICLE_TYPE >& src );

      /** Get the history of the given particle */
      ParticleHistory&  getHistory( uint index ) const { return currentHistory[ index ]; }

      /** Return the number of time steps present for each particle */
      uint getNumberOfTimeSteps() const;

      /** Return the number of stored particle histories */
      uint size() const { return m_uFilterSize; }

    public:
      ParticleHistory* currentHistory;
      ParticleHistory* previousHistory;
      uint             m_uFilterSize;         ///< Filter size, i.e., number of particles
      uint             m_uMostLikelyParticle; ///< Index of the currently most likely particle
  };

}; // end namespace pf2


#include "particle_filter_history.cpp"

#endif // AMBISENSE_SLAM__FILTERS__PARTICLE_FILTER_HISTORY_H
