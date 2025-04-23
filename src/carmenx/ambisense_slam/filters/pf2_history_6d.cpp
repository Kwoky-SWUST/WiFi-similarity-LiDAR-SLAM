#include <carmen/carmen.h>
#include <ambisense_slam_config.h>
#include <motion_models/motion_model.h>
#include "pf2_history_6d.h"
//
#include <utils/quality/sciroco_exception.h>

using namespace std;


// --------------------------------------------------------------------------
pf2::ParticleFilterHistory6D::ParticleHistory
pf2::ParticleFilterHistory6D::
    smooth( const MotionModel* /*m*/, SmoothingMethod method )
// --------------------------------------------------------------------------
{
  assert( currentHistory != NULL );
  switch ( method )
  {
    case SIMPLE_WEIGHTING_SMOOTHING:  return smoothingBySimpleWeighting();
    case MOST_LIKELY_PARTICLE:        return smoothingByMostLikelyParticle();
//     case BACKWARD_SMOOTHING:          return smoothingByBackwardSmoothing( m );
    default:
      throw ScirocoException( "Request for unknown or not yet implemented smoothing method", ScirocoException::NOT_YET_IMPLEMENTED );
      break;
  }
  return ParticleHistory();
}

static map< int, double > m_mCachedTransitionProbs;

// --------------------------------------------------------------------------
double pf2::ParticleFilterHistory6D::
getTransitionProbability( const MotionModel* motionModel,
                          const carmen_point_t& transition,
                          const carmen_point_t& odo )
// --------------------------------------------------------------------------
{
  double prob;
  static const double discrete_xy    = 0.05;
  static const double discrete_theta = 0.05;

  double tx = transition.x - odo.x;
  double ty = transition.y - odo.y;
  double tt = transition.theta - odo.theta;

  int index = lrint( tx / discrete_xy ) * 1000000
      + lrint( ty / discrete_xy ) * 1000
      + lrint( tt / discrete_theta );
  map<int, double>::iterator iter = m_mCachedTransitionProbs.find( index );
  if ( iter == m_mCachedTransitionProbs.end() )
  {
    prob = motionModel->getProbability( transition, odo );
    m_mCachedTransitionProbs[ index ] = prob;
  }
  else
    prob = iter->second;

  return prob;
}

// --------------------------------------------------------------------------
void pf2::ParticleFilterHistory6D::
    smoothBackward( const MotionModel* motionModel,
                    const std::vector< carmen_point_t >& odometry
                  )
// --------------------------------------------------------------------------
{
  cout << "debug: " << __FILE__ << ":" << __FUNCTION__ << endl;
  double start_time = carmen_get_time();

  assert( currentHistory != NULL );
  assert( previousHistory != NULL );

  // Use previousHistory as buffer for newly sampled history
  ParticleHistory* new_history = previousHistory;
  uint total_time = currentHistory[0].size();
  if ( total_time < 2 )
  {
    cerr << "WARNING: Backward smoothing not possible for fewer than 2 time steps!" << endl;
    return;
  }
  assert( odometry.size() == total_time - 1 );

/*  double* acc_smoothing_weights = new double[ m_uFilterSize ];*/
  static vector<double> acc_smoothing_weights( m_uFilterSize );
  carmen_point_t displacement_t_t_next;
  double dx, dy, _cos, _sin, smoothing_weight_sum, transition_prob;
  int    t;
  uint   sampled_index, p;
  for ( uint new_sample = 0; new_sample < m_uFilterSize; ++new_sample )
  {
    cout << '*';
    cout << "debug: " << __FUNCTION__ << ":" << __LINE__ << ": new_sample=" << new_sample << "  \r" << flush;

    ParticleHistory& sampled_history = new_history[ new_sample ];
    sampled_history.resize( total_time );

    // The first step, i.e., most recent entry of the history,
    // has already been determined via resampling:
    sampled_history[ total_time - 1 ] = currentHistory[ new_sample ][ total_time - 1 ];
    sampled_index = new_sample;

    for ( t = total_time - 2; t >= 0; t-- )
    {
//       cout << "debug: " << __FUNCTION__ << ":" << __LINE__ << ": t=" << t << "  \r" << flush;

      Particle& particle_next = currentHistory[ sampled_index ][ t+1 ];
      smoothing_weight_sum = 0.0;

//       cout << "  particle_next: " << particle_next.x << ", " << particle_next.y << ", " << particle_next.theta_z << '\n';

      for ( p = 0; p < m_uFilterSize; ++p )
      {
        Particle& particle_t = currentHistory[p][t];
//         cout << "  particle_t:    " << particle_t.x << ", " << particle_t.y << ", " << particle_t.theta_z << '\n';

        // Compute displacement between particle pos at earlier time step (t)
        // and later time step (t+1)
        dx     = particle_next.x - particle_t.x;
        dy     = particle_next.y - particle_t.y;
        _cos = cos( -particle_t.theta_z );
        _sin = sin( -particle_t.theta_z );
        displacement_t_t_next.theta = carmen_normalize_theta( particle_next.theta_z - particle_t.theta_z );
        displacement_t_t_next.x = _cos * dx - _sin * dy;
        displacement_t_t_next.y = _sin * dx + _cos * dy;

        // Compute transition probability for this displacement
//         cout << "Probed displacement: " << displacement_t_t_next.x << ", " << displacement_t_t_next.y << ", " << displacement_t_t_next.theta << '\n';
//         cout << "           Odometry: " << odometry[t].x << ", " << odometry[t].y << ", " << odometry[t].theta << '\n';
        transition_prob = getTransitionProbability( motionModel, displacement_t_t_next, odometry[t] );

        smoothing_weight_sum += transition_prob * particle_t.weight;
        cout << "_______ Muss hier wirklich particle_t.weight stehen und nicht quasi PF[ new_sample ].weight?" << endl;
        acc_smoothing_weights[p] = smoothing_weight_sum;
      }

      // Sample new index according to (accumulated) smoothing weights
      sampled_index = lower_bound( acc_smoothing_weights.begin(),
                                   acc_smoothing_weights.end(),
                                   carmen_uniform_random( 0.0, smoothing_weight_sum ) ) - acc_smoothing_weights.begin();
      cout << "__________ Smoothed Trajektorien mal ausgeben lassen" << endl;
      cout << "Hier Zeitschritte auslassen" << endl;
      exit(0);

      assert( sampled_index < m_uFilterSize );
      sampled_history[t] = currentHistory[ sampled_index ][t];
      // That's it...
    }
  }
  cout << "debug: " << __FUNCTION__ << ":" << __LINE__ << "  duration: " << carmen_get_time() - start_time << endl;

  previousHistory = currentHistory;
  currentHistory = new_history;

//   delete [] acc_smoothing_weights;
}


// --------------------------------------------------------------------------
pf2::ParticleFilterHistory6D::ParticleHistory
pf2::ParticleFilterHistory6D::
    smoothingBySimpleWeighting()
// --------------------------------------------------------------------------
{
  uint time_steps = currentHistory[0].size();
  ParticleHistory history;
  history.resize( time_steps );
  pf2::Particle6D p;
  double cos_theta_z, sin_theta_z;
  double weight;
  for ( uint t = 0; t < time_steps; ++t )
  {
    cos_theta_z = sin_theta_z = 0.0;
    history[t].x = 0.0;
    history[t].y = 0.0;
    for ( uint p = 0; p < m_uFilterSize; ++p )
    {
      Particle& particle = currentHistory[p][t];
      weight = currentHistory[p][time_steps-1].weight;
      history[t].x += weight * particle.x;
      history[t].y += weight * particle.y;
      cos_theta_z += weight * cos( particle.theta_z );
      sin_theta_z += weight * sin( particle.theta_z );
    }
    history[t].theta_z = atan2( sin_theta_z, cos_theta_z );
  }
  return history;
}


// --------------------------------------------------------------------------
pf2::ParticleFilterHistory6D::ParticleHistory
pf2::ParticleFilterHistory6D::
    smoothingByMostLikelyParticle()
// --------------------------------------------------------------------------
{
  uint time_steps = currentHistory[0].size();
  ParticleHistory history;
  history.resize( time_steps );
  pf2::Particle6D p;
  for ( uint t = 0; t < time_steps; ++t )
  {
    Particle& particle = currentHistory[ m_uMostLikelyParticle ][t];
    history[t] = particle;
  }
  return history;
}


// --------------------------------------------------------------------------
std::vector< carmen_point_t >
pf2::ParticleFilterHistory6D::
    get2DTrajectory( uint index ) const
// --------------------------------------------------------------------------
{
  vector< carmen_point_t > estimated_trajectory;
  for ( uint u = 0; u < currentHistory[ index ].size(); ++u )
    estimated_trajectory.push_back( (carmen_point_t) { currentHistory[ index ][u].x, currentHistory[ index ][u].y, currentHistory[ index ][u].theta_z } );
  return estimated_trajectory;
}
