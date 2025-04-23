#include <carmen/carmen.h>
#include <ambisense_slam_config.h>
#include "pf2_xytheta.h"
// #include <mcl_random/fast_gauss_rand.h>
//
#include <newmat/newmat.h>
#include <newmat/newmatap.h>
#include <fstream>
#include <iostream>
#include <iomanip>
// iostream+iomanip must be included before newmatio.h:
#include <newmat/newmatio.h>

using namespace std;


// --------------------------------------------------------------------------
void
pose2matrixXYTheta( const Pose&p, Matrix& M )
// --------------------------------------------------------------------------
{
  assert( M.Nrows() == 6 );
  M(1,1) = p.x;
  M(2,1) = p.y;
  M(3,1) = 0;
  M(4,1) = 0;
  M(5,1) = 0;
  M(6,1) = p.theta_z;
}

/*
// --------------------------------------------------------------------------
pf2::ParticleXYTheta::
    ParticleXYTheta()
// --------------------------------------------------------------------------
{
}


// // --------------------------------------------------------------------------
// pf2::ParticleXYTheta::
//     ~ParticleXYTheta()
// // --------------------------------------------------------------------------
// {
// #ifdef HAVE_PARTICLE_TRAJECTORY
//   trajectory.clear();
// #endif
// }


// --------------------------------------------------------------------------
pf2::ParticleXYTheta::
    ParticleXYTheta( const pf2::ParticleXYTheta& p ) : carmen_point_t()
// --------------------------------------------------------------------------
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
        theta            = p.theta;
        weight             = p.weight;
#ifdef HAVE_LOG_WEIGHTS
        log_weight         = p.log_weight;
#endif
}


// --------------------------------------------------------------------------
pf2::ParticleXYTheta::
    ParticleXYTheta( const carmen_6d_point_t& p ) : carmen_point_t()
// --------------------------------------------------------------------------
{
  setPosition( p );
}


// --------------------------------------------------------------------------
carmen_ambisense_particle_ipc_t pf2::ParticleXYTheta::
    asIPCParticle() const
// --------------------------------------------------------------------------
{
  carmen_ambisense_particle_ipc_t p;
  p.x = x;
  p.y = y;
  p.z = 0;
  p.theta  = theta;
  p.weight = weight;
  p.accumulated_weight = 0.0;

  return p;
}


// --------------------------------------------------------------------------
void pf2::ParticleXYTheta::
    setPosition( const carmen_point_t& p )
// --------------------------------------------------------------------------
{
  x     = p.x;
  y     = p.y;
  theta = p.theta;
}


// --------------------------------------------------------------------------
void pf2::ParticleXYTheta::
    setPosition( const carmen_6d_point_t& p )
// --------------------------------------------------------------------------
{
  x = p.x;
  y = p.y;
  theta = p.theta_z;
}
*/

// --------------------------------------------------------------------------
pf2::ParticleFilterXYTheta::
    ParticleFilterXYTheta()
  : pf2::ParticleFilter< ParticleXYTheta >()
// --------------------------------------------------------------------------
{
}


// --------------------------------------------------------------------------
void pf2::ParticleFilterXYTheta::
    addGaussianNoise( FastGaussRand< double >* generator,
                      const double translationStdDev,
                      const double orientationStdDev )
// --------------------------------------------------------------------------
{
  GaussNumberPair< double > pair;

  if ( translationStdDev > 0 )
  {
    for ( uint i = 0; i < particles.size(); ++i )
    {
      pair = generator->getRandomNumberPair();

      particles[i].x += pair.value[0] * translationStdDev;
      particles[i].y += pair.value[1] * translationStdDev;
    }
  }

  if ( orientationStdDev > 0 )
  {
    for ( uint i = 0; i < particles.size(); ++i )
    {
      if ( ! (i%2) )
        pair = generator->getRandomNumberPair();

      particles[i].theta += pair.value[i % 2] * orientationStdDev;
    }
  }
}


// --------------------------------------------------------------------------
void pf2::ParticleFilterXYTheta::
    computeMeanAndVariance2D( Pose   & mean,
                              Pose   & variance,
                              double & xy_covariance )
// --------------------------------------------------------------------------
{
  mean = variance = Pose( 0,0,0,0,0,0 );
  xy_covariance = 0.0;
  double theta_x = 0.0, theta_y = 0.0;
  if ( size() == 0 )
    return;

  for ( std::vector< ParticleXYTheta >::const_iterator it = particles.begin(); it != particles.end(); ++it )
  {
    mean.x  += it->x * it->weight;
    mean.y  += it->y * it->weight;
    theta_x += cos( it->theta ) * it->weight;
    theta_y += sin( it->theta ) * it->weight;
  }
  mean.theta_z = atan2( theta_y, theta_x );
}


// --------------------------------------------------------------------------
void pf2::ParticleFilterXYTheta::
    computeMean( Pose& mean )
// --------------------------------------------------------------------------
{
  mean = (carmen_point_t) { 0,0,0 };
  double theta_x = 0.0, theta_y = 0.0;
  if ( size() == 0 )
    return;

/*  ParticleXYTheta* p;
  for ( uint u = 0; u < particles.size(); ++u )
  {
    p = &( particles[u] );
    mean.x  += p->x * p->weight;
    mean.y  += p->y * p->weight;
    mean.z  += p->z * p->weight;
    theta_x += cos( p->theta_z ) * p->weight;
    theta_y += sin( p->theta_z ) * p->weight;
  }*/
  for ( std::vector< ParticleXYTheta >::const_iterator it = particles.begin(); it != particles.end(); ++it )
  {
    mean.x  += it->x * it->weight;
    mean.y  += it->y * it->weight;
    theta_x += cos( it->theta ) * it->weight;
    theta_y += sin( it->theta ) * it->weight;
  }
  mean.theta_z = atan2( theta_y, theta_x );
}


// --------------------------------------------------------------------------
void pf2::ParticleFilterXYTheta::
    computeMeanAndCovariance( Pose& mean, double covariance[36] )
// --------------------------------------------------------------------------
{
  computeMean( mean );

  if ( size() == 0 )
    return;

  Matrix Mean(6,1), X(6,1), D;
  pose2matrixXYTheta( mean, Mean );
  Matrix C(6,6);

  // Set all values of C zero
  C = 0.0;
  X = 0.0;

  for ( std::vector< ParticleXYTheta >::const_iterator it = particles.begin(); it != particles.end(); ++it )
  {
    X(1,1) = it->x;
    X(2,1) = it->y;
    X(6,1) = it->theta;
    D  = X - Mean;
    C += D * D.t() * it->weight;
  }
//   C *= 1.0 / particles.size();

  for ( int i = 0; i < 6; ++i )
    for ( int j = 0; j < 6; ++j )
      covariance[ i*6+j ] = C( i+1, j+1 );
}


// --------------------------------------------------------------------------
std::ostream & operator << ( std::ostream & ostr, const pf2::ParticleFilterXYTheta & pf )
// --------------------------------------------------------------------------
{
  for ( std::vector< pf2::ParticleXYTheta >::const_iterator it = pf.particles.begin(); it != pf.particles.end(); ++it )
  {
    ostr << "(" << it->x << ", " << it->y << ")\n";
  }
  return ostr;
}
