#include <carmen/carmen.h>
//
#include <carmenx/ambisense_slam/ambisense_slam_interface.h>
#include <ambisense_slam_config.h>
//
#include "particle_filter.h"
// #include "pf2_6d.h"
#include "particle_filter_2.h"
//
#include <fstream>
#include <iostream>

using namespace std;

const unsigned int NUM_SAMPLES = 100000;
const unsigned int NUM_NORMALIZATIONS_AND_RESAMPLINGS = 1000;

// class BenchmarkParticle : public carmen_point_t
// {
//   public:
//     BenchmarkParticle& operator =( BenchmarkParticle p ) 
//     { 
// //       memcpy( this, &p, sizeof( BenchmarkParticle ) );
//       x = p.x; 
//       y = p.y; 
// //       z = p.z; 
// //       theta_x = p.theta_x; 
// //       theta_y = p.theta_y; 
// //       theta_z = p.theta_z; 
//       theta = p.theta; 
//       return *this; 
//     }
// 
//     double weight;
//     double log_weight;
// };
// typedef pf2::ParticleXYTheta BenchmarkParticle;
typedef pf2::Particle6D            BenchmarkParticle;
// typedef carmen_ambisense_particle_ipc_t            BenchmarkParticle;

/// The gaussian random number generator supplied to the particle filters
// static FastGaussRand< double > * m_pFastGaussRand;

typedef ParticleFilter< BenchmarkParticle, double >         PF1;
typedef pf2::ParticleFilter< BenchmarkParticle >            PF2;


#if 0
// --------------------------------------------------------------------------
void benchmark_pf1()
// --------------------------------------------------------------------------
{
  m_pFastGaussRand = new FastGaussRand< double >( 0.0, 1.0, 30000 );
  PF1 dummy_particle_filter( NUM_SAMPLES, m_pFastGaussRand );

  double last_time = carmen_get_time(), current_time;

  BenchmarkParticle p;
  p.theta = p.z = p.accumulated_weight = 0.0;
  p.weight = 1.0 / NUM_SAMPLES;
#ifdef HAVE_LOG_WEIGHTS
  p.log_weight = log( p.weight );
#endif
  for ( unsigned int i = 0; i < NUM_SAMPLES; ++i )
  {
    p.x = carmen_gaussian_random( 0.0, 1.0 );
    p.y = carmen_gaussian_random( 0.0, 1.0 );
    dummy_particle_filter.addParticle( p );
  }

  current_time = carmen_get_time();
  cout << "Adding particles: " << (current_time - last_time) << endl;

  last_time = carmen_get_time();
  double acc_resampling_time = 0.0;
  for ( uint i = 0; i < NUM_NORMALIZATIONS_AND_RESAMPLINGS; ++i )
  {
    dummy_particle_filter.normalize();
//     cout << dummy_particle_filter;

//     cout << "effectiveSampleSetSize: " << dummy_particle_filter.getEffectiveSampleSize() << endl;
//     cin >> argc;

    current_time = carmen_get_time();
//     cout << "Normalization: " << (current_time - last_time) << endl;
    last_time = current_time;

    dummy_particle_filter.resample();
    current_time = carmen_get_time();
//     cout << "Resampling:    " << (current_time - last_time) << endl;
    acc_resampling_time += (current_time - last_time);
    last_time = current_time;
  }
  cout << "Mean resampling time: " << acc_resampling_time / NUM_NORMALIZATIONS_AND_RESAMPLINGS << endl;
}
#endif


// --------------------------------------------------------------------------
void benchmark_pf2_access()
// --------------------------------------------------------------------------
{
  PF2 dummy_particle_filter;

  double last_time = carmen_get_time(), current_time;

  BenchmarkParticle p;
//   p.theta = p.z = p.accumulated_weight = 0.0;
//   p.theta_z = p.z = 0.0;
  p.weight = 1.0 / NUM_SAMPLES;
#ifdef HAVE_LOG_WEIGHTS
  p.log_weight = log( p.weight );
#endif
  for ( unsigned int i = 0; i < NUM_SAMPLES; ++i )
  {
    p.x = carmen_gaussian_random( 0.0, 1.0 );
    p.y = carmen_gaussian_random( 0.0, 1.0 );
    dummy_particle_filter.addParticle( p );
  }

  current_time = carmen_get_time();
  cout << "Adding particles: " << (current_time - last_time) << endl;

  last_time = carmen_get_time();
  const uint NUM_ACCESSES = 9999;
  unsigned long int count = 0;
  for ( uint i = 0; i < NUM_ACCESSES; ++i )
  {
    vector<BenchmarkParticle>& ps = dummy_particle_filter.getParticles();
    for ( uint u = 0; u < dummy_particle_filter.size(); ++u )
    {
      BenchmarkParticle& pp = ps[u];
      ++count;
    }
  }
  current_time = carmen_get_time();
  cout << "\n Duration of " << count << " accesses: " << current_time - last_time << " seconds" << endl;
}

#if 0
// --------------------------------------------------------------------------
void benchmark_pf2()
// --------------------------------------------------------------------------
{
  PF2 dummy_particle_filter;

  double last_time = carmen_get_time(), current_time;

  BenchmarkParticle p;
  p.theta = p.z = p.accumulated_weight = 0.0;
  p.weight = 1.0 / NUM_SAMPLES;
#ifdef HAVE_LOG_WEIGHTS
  p.log_weight = log( p.weight );
#endif
  for ( unsigned int i = 0; i < NUM_SAMPLES; ++i )
  {
    p.x = carmen_gaussian_random( 0.0, 1.0 );
    p.y = carmen_gaussian_random( 0.0, 1.0 );
    dummy_particle_filter.addParticle( p );
  }

  current_time = carmen_get_time();
  cout << "Adding particles: " << (current_time - last_time) << endl;

  last_time = carmen_get_time();
  double acc_resampling_time = 0.0;
  for ( uint i = 0; i < NUM_NORMALIZATIONS_AND_RESAMPLINGS; ++i )
  {
    dummy_particle_filter.normalize();
//     cout << dummy_particle_filter;

//     cout << "effectiveSampleSetSize: " << dummy_particle_filter.getEffectiveSampleSize() << endl;
//     cin >> argc;

    current_time = carmen_get_time();
//     cout << "Normalization: " << (current_time - last_time) << endl;
    last_time = current_time;

    dummy_particle_filter.resample( PF2::RESIDUAL_RESAMPLING );
    current_time = carmen_get_time();
//     cout << "Resampling:    " << (current_time - last_time) << endl;
    acc_resampling_time += (current_time - last_time);
    last_time = current_time;
  }
  cout << "Mean resampling time: " << acc_resampling_time / NUM_NORMALIZATIONS_AND_RESAMPLINGS << endl;
}
#endif


// --------------------------------------------------------------------------
int main( int /*argc*/, char** /*argv*/ )
// --------------------------------------------------------------------------
{
//   benchmark_pf1();
  benchmark_pf2_access();
//   benchmark_pf2();

  return 0;
}
