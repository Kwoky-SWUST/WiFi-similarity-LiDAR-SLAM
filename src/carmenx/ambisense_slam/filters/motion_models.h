#include <mcl_random/fast_gauss_rand.h>

namespace pf2
{
  template< class PARTICLE_TYPE >
  class MotionModelCarmenPointT
  {
    public:
      /** Constructor */
      MotionModelCarmenPointT< PARTICLE_TYPE >();
      void apply
          ( ParticleFilter< PARTICLE_TYPE > & pf,
            double odo_dx,  double odo_dy,
            double dtheta1, double dtheta2,
            double mean_trans,
            double mean_rot,
            double stddev_trans_trans,
            double stddev_trans_rot,
            double stddev_rot_trans,
            double stddev_rot_rot
          );

    protected:
      FastGaussRand<double> * m_pFastGaussRand;
  };
};


// --------------------------------------------------------------------------
template< class PARTICLE_TYPE >
pf2::MotionModelCarmenPointT< PARTICLE_TYPE >::
    MotionModelCarmenPointT()
// --------------------------------------------------------------------------
{
  m_pFastGaussRand = new FastGaussRand<double>( 0.0, 1.0, 30000 );
}


// --------------------------------------------------------------------------
template< class PARTICLE_TYPE >
void pf2::MotionModelCarmenPointT< PARTICLE_TYPE >::
    apply( ParticleFilter< PARTICLE_TYPE > & pf,
            double odo_dx,  double odo_dy,
            double dtheta1, double dtheta2,
            double mean_trans,
            double /*mean_rot*/,
            double stddev_trans_trans,
            double stddev_trans_rot,
            double /*stddev_rot_trans*/,
            double stddev_rot_rot
          )
// --------------------------------------------------------------------------
{
  double _cos, _sin;
  double dx, dy;

  // For the addition of noise, we need to determine the travelled distance
  // and the rotation which has been performed since the last invocation
  double len = sqrt( odo_dx * odo_dx + odo_dy * odo_dy );

  double len_factor_noisy, noisy_len, theta1, theta2;

  // Iterate over all localization particles
  typedef typename std::vector< PARTICLE_TYPE >::iterator Iter;
  for ( Iter it = pf.particles.begin(); it != pf.particles.end(); ++it )
  {
    theta1 = dtheta1 * ( 1 + stddev_rot_rot * m_pFastGaussRand->getRandomNumber() );

    len_factor_noisy = mean_trans + stddev_trans_trans * m_pFastGaussRand->getRandomNumber();
    noisy_len = len * len_factor_noisy;

    _cos = cos( it->theta + theta1 );
    _sin = sin( it->theta + theta1 );

    dx = ( _cos * odo_dx - _sin * odo_dy ) * len_factor_noisy;
    dy = ( _sin * odo_dx + _cos * odo_dy ) * len_factor_noisy;

    theta2 = dtheta2 * ( 1 + stddev_rot_rot * m_pFastGaussRand->getRandomNumber() ) + noisy_len * stddev_trans_rot * m_pFastGaussRand->getRandomNumber();

    it->x     += dx;
    it->y     += dy;
    it->theta += ( theta1 + theta2 );
  }

}


namespace pf2 {
  // --------------------------------------------------------------------------
  template< >
  void MotionModelCarmenPointT< Particle6D >::
      apply( ParticleFilter< Particle6D > & pf,
              double odo_dx,  double odo_dy,
              double dtheta1, double dtheta2,
              double mean_trans,
              double /*mean_rot*/,
              double stddev_trans_trans,
              double stddev_trans_rot,
              double /*stddev_rot_trans*/,
              double stddev_rot_rot
            )
  // --------------------------------------------------------------------------
  {
    double _cos, _sin;
    double dx, dy;

    // For the addition of noise, we need to determine the travelled distance
    // and the rotation which has been performed since the last invocation
    double len = sqrt( odo_dx * odo_dx + odo_dy * odo_dy );

    double len_factor_noisy, noisy_len, theta1, theta2;

    // Iterate over all localization particles
    typedef typename std::vector< Particle6D >::iterator Iter;
    for ( Iter it = pf.particles.begin(); it != pf.particles.end(); ++it )
    {
      theta1 = dtheta1 * ( 1 + stddev_rot_rot * m_pFastGaussRand->getRandomNumber() );

      len_factor_noisy = mean_trans + stddev_trans_trans * m_pFastGaussRand->getRandomNumber();
      noisy_len = len * len_factor_noisy;

      _cos = cos( it->theta_z + theta1 );
      _sin = sin( it->theta_z + theta1 );

      dx = ( _cos * odo_dx - _sin * odo_dy ) * len_factor_noisy;
      dy = ( _sin * odo_dx + _cos * odo_dy ) * len_factor_noisy;

      theta2 = dtheta2 * ( 1 + stddev_rot_rot * m_pFastGaussRand->getRandomNumber() ) + noisy_len * stddev_trans_rot * m_pFastGaussRand->getRandomNumber();

      it->x     += dx;
      it->y     += dy;
      it->theta_z += ( theta1 + theta2 );
    }
  }
};

