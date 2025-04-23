#include <carmen/carmen.h>
#include <motion_models/crossrange_downrange_rotation_motion_model.h>
#include <utils/quality/sciroco_exception.h>
#include <utils/math/math.h>

using namespace std;


// --------------------------------------------------------------------------
CrossrangeDownrangeRotationMotionModel::
    CrossrangeDownrangeRotationMotionModel( MotionModelParameters* mmParams,
                                            FastGaussRand<double>*  rand)
  : MotionModel( mmParams, "crossrange_downrange_rotation" )
// --------------------------------------------------------------------------
{
  m_pFastGaussRand = rand;
  ScirocoException::test( m_pFastGaussRand != NULL,
                          "Gaussian random number generator is NULL",
                          __FUNCTION__, __LINE__ );
}


// --------------------------------------------------------------------------
CrossrangeDownrangeRotationMotionModel::
    ~CrossrangeDownrangeRotationMotionModel()
// --------------------------------------------------------------------------
{
}


inline double gaussian( double x, double s )
{
  static const double f = 1 / sqrt( 2 * M_PI );
  return exp( - 0.5 * x * x / ( s * s ) ) / s * f;
}


// --------------------------------------------------------------------------
double CrossrangeDownrangeRotationMotionModel::
      getProbability( const carmen_point_t& transformation,
                      const carmen_point_t& odometry ) const
// --------------------------------------------------------------------------
{
  double _cos = cos( -0.5*transformation.theta ), _sin = sin( -0.5*transformation.theta );
  double d = _cos * transformation.x - _sin * transformation.y;
  double c = _sin * transformation.x + _cos * transformation.y;
  double r = transformation.theta;

  // Measured translation and rotation
  double translation = sqrt( odometry.x*odometry.x + odometry.y*odometry.y );
  double rotation    = fabs( odometry.theta );

  double expected_downrange  = translation * params->downrangeMeanByTrans  + rotation * params->downrangeMeanByRot;
  double expected_crossrange = translation * params->crossrangeMeanByTrans + rotation * params->crossrangeMeanByRot;
  double expected_rotation   = translation * params->rotationMeanByTrans   + rotation * params->rotationMeanByRot;

  double downrange_std_dev = fabs( translation * params->downrangeStdDevByTrans )
        + fabs( rotation * params->downrangeStdDevByRot );

  double crossrange_std_dev = fabs( translation * params->crossrangeStdDevByTrans )
        + fabs( rotation * params->crossrangeStdDevByRot );

  double rotation_std_dev = fabs( translation * params->rotationStdDevByTrans )
        + fabs( rotation    * params->rotationStdDevByRot );

  double dd = d - expected_downrange;
  double dc = c - expected_crossrange;
  double dr = r - expected_rotation;

  double p_c, p_d, p_r;
  p_d = gaussian( dd, downrange_std_dev );
  p_c = gaussian( dc, crossrange_std_dev );
  p_r = gaussian( dr, rotation_std_dev );

  return p_c * p_d * p_r;
}


// --------------------------------------------------------------------------
void CrossrangeDownrangeRotationMotionModel::
    apply( carmen_point_t         odometry,
           double                 timeDifference,
           unsigned int           localizationStep,
           std::vector<Particle>& particles )
// --------------------------------------------------------------------------
{
  double translation = sqrt( odometry.x*odometry.x + odometry.y*odometry.y );
  if ( odometry.x < 0.0 )
    translation *= -1;

  double rotation    = odometry.theta;

  double noisy_translation = translation
                           + fabs( translation * params->config_artificial_noise_dist_per_dist ) * m_pFastGaussRand->getRandomNumber()
                           + fabs( rotation * params->config_artificial_noise_dist_per_rot ) * m_pFastGaussRand->getRandomNumber();

  double noisy_rotation = rotation
                        + fabs( translation * params->config_artificial_noise_rot_per_dist ) * m_pFastGaussRand->getRandomNumber()
                        + fabs( rotation * params->config_artificial_noise_rot_per_rot ) * m_pFastGaussRand->getRandomNumber();

  propagateParticles( particles,
                      noisy_translation, noisy_rotation, timeDifference, localizationStep );
/*  double   dtheta1, dtheta2;

  // Two rotations: one before, one after translation
  dtheta1 = atan2( odometry.y, odometry.x );
  dtheta2 = carmen_normalize_theta( odometry.theta - dtheta1 );

  double sqr_len = odometry.x*odometry.x + odometry.y*odometry.y;
  if ( sqr_len < 0.1*0.1 )
  {
    dtheta1 = 0.0;
    dtheta2 = odometry.theta;
  }

  // Correct odometry X/Y, because translation will be in direction dtheta1
  double _cos = cos( -dtheta1 );
  double _sin = sin( -dtheta1 );
  double dx = _cos * odometry.x - _sin * odometry.y;
  double dy = _sin * odometry.x + _cos * odometry.y;
  odometry.x = dx;
  odometry.y = dy;

  noisify( odometry.x, odometry.y, dtheta1, dtheta2, timeDifference, localizationStep );
  propagateParticlesWRONG( particles,
                           odometry.x, odometry.y, dtheta1, dtheta2, timeDifference, localizationStep );*/
}


// --------------------------------------------------------------------------
void CrossrangeDownrangeRotationMotionModel::
    noisify(  double& odo_dx,
              double& odo_dy,
              double& dtheta1,
              double& dtheta2,
              double& /*timeDifference*/,
              unsigned int& /*localizationStep*/ )
// --------------------------------------------------------------------------
{
  double len = sqrt( odo_dx*odo_dx + odo_dy*odo_dy );
  double abs_rot = fabs( dtheta1 ) + fabs( dtheta2 );

  // Add artificial noise if desired //////////////////////
  if ( params->config_artificial_noise_dist_per_dist > 0.0 )
  {
    double lr = 1.0 + m_pFastGaussRand->getRandomNumber() * params->config_artificial_noise_dist_per_dist * len; //carmen_gaussian_random( 1.0, config_artificial_noise_dist_per_dist * len );
//     cout << "_________________________ lr=" << lr << endl;
    odo_dx *= lr;
    odo_dy *= lr;
  }
  if ( params->config_artificial_noise_rot_per_dist > 0.0 )
  {
    double r1 = m_pFastGaussRand->getRandomNumber() * params->config_artificial_noise_rot_per_dist * len; //carmen_gaussian_random( 0.0, config_artificial_noise_rot_per_dist * len );
    double r2 = m_pFastGaussRand->getRandomNumber() * params->config_artificial_noise_rot_per_dist * len;//carmen_gaussian_random( 0.0, config_artificial_noise_rot_per_dist * len );
//     cout << "_________________________" << r1 << "/" << r2 << endl;
    dtheta1 += r1;
    dtheta2 += r2;
  }
  if ( params->config_artificial_noise_dist_per_rot > 0.0 )
  {
    double lr = 1.0 + m_pFastGaussRand->getRandomNumber() * params->config_artificial_noise_dist_per_rot * abs_rot;
//     double lr = carmen_gaussian_random( 1.0, config_artificial_noise_dist_per_rot * abs_rot );
//     cout << "_________________________ lr=" << lr << endl;
    odo_dx *= lr;
    odo_dy *= lr;
  }
  if ( params->config_artificial_noise_rot_per_rot > 0.0 )
  {
    double r1 = m_pFastGaussRand->getRandomNumber() * params->config_artificial_noise_rot_per_rot * abs_rot; //carmen_gaussian_random( 0.0, config_artificial_noise_rot_per_rot * abs_rot );
    double r2 = m_pFastGaussRand->getRandomNumber() * params->config_artificial_noise_rot_per_rot * abs_rot; //carmen_gaussian_random( 0.0, config_artificial_noise_rot_per_rot * abs_rot );
//     cout << "_________________________" << r1 << "/" << r2 << endl;
    dtheta1 += r1;
    dtheta2 += r2;
  }
  // END of artificial noise //////////////////////////////
}


// --------------------------------------------------------------------------
void CrossrangeDownrangeRotationMotionModel::
propagateParticlesWRONG( std::vector<Particle>& particles,
                        double odo_dx,
                        double odo_dy,
                        double dtheta1,
                        double dtheta2,
                        double /*timeDifference*/,
                        unsigned int localizationStep )
// --------------------------------------------------------------------------
{
  VERBOSE_OUT << color::purple << "Odometry CROSSRANGE_DOWNRANGE_ROTATION: x=" << odo_dx << ",  y=" << odo_dy << ", dtheta1=" << dtheta1 << ", dtheta2=" << dtheta2 << color::normal << endl;

  // Special noise for first, global localization steps
  double fr = params->config_localization_noise_rot_discount_factor  * exp( - params->config_localization_noise_rot_discount_rate  * localizationStep );
  double fd = params->config_localization_noise_dist_discount_factor * exp( - params->config_localization_noise_dist_discount_rate * localizationStep );

  double downrange, crossrange, rotation, rot_1, rot_2;
  double dx, dy, _cos, _sin;

  // For the addition of noise, we need to determine the travelled distance
  // and the rotation which has been performed since the last invocation
  double len = sqrt( odo_dx * odo_dx + odo_dy * odo_dy );
//   double rot_total = fabs( dtheta1 ) + fabs( dtheta2 );

  double crossrangeStdDevByTrans = ( params->crossrangeStdDevByTrans );
  double crossrangeStdDevByRot = ( params->crossrangeStdDevByRot );
  double downrangeStdDevByTrans = ( params->downrangeStdDevByTrans ) + fd;
  double downrangeStdDevByRot = ( params->downrangeStdDevByRot );
  double rotationStdDevByTrans = ( params->rotationStdDevByTrans );
  double rotationStdDevByRot = ( params->rotationStdDevByRot ) + fr;

  // Iterate over all localization particles
  for ( uint p = 0; p < particles.size(); ++p )
  {
    Particle& particle = particles[p];

    // First rotation
    rot_1 = dtheta1 * ( params->rotationMeanByRot + m_pFastGaussRand->getRandomNumber() * rotationStdDevByRot );

    _cos = cos( particle.theta_z + rot_1 );
    _sin = sin( particle.theta_z + rot_1 );

    downrange  = len * ( params->downrangeMeanByTrans + m_pFastGaussRand->getRandomNumber() * downrangeStdDevByTrans )
          + dtheta1 * ( params->downrangeMeanByRot + m_pFastGaussRand->getRandomNumber() * downrangeStdDevByRot );

    crossrange  = len * ( params->crossrangeMeanByTrans + m_pFastGaussRand->getRandomNumber() * crossrangeStdDevByTrans )
        + dtheta1 * ( params->crossrangeMeanByRot + m_pFastGaussRand->getRandomNumber() * crossrangeStdDevByRot );

    rotation  = len * ( params->rotationMeanByTrans + m_pFastGaussRand->getRandomNumber() * rotationStdDevByTrans )
        + rot_1;

    dx = _cos * downrange - _sin * crossrange;
    dy = _sin * downrange + _cos * crossrange;

    particle.x       += dx;
    particle.y       += dy;
    particle.theta_z = particle.theta_z + rot_1; // normalization is performed later, below!

    // Final rotation
    rot_2 = dtheta2 * ( params->rotationMeanByRot + m_pFastGaussRand->getRandomNumber() * rotationStdDevByRot );

    _cos = cos( particle.theta_z + rot_2 );
    _sin = sin( particle.theta_z + rot_2 );

    downrange  = dtheta2 * ( params->downrangeMeanByRot  + m_pFastGaussRand->getRandomNumber() * downrangeStdDevByRot );
    crossrange = dtheta2 * ( params->crossrangeMeanByRot + m_pFastGaussRand->getRandomNumber() * crossrangeStdDevByRot );

    dx = _cos * downrange - _sin * crossrange;
    dy = _sin * downrange + _cos * crossrange;

    particle.x       += dx;
    particle.y       += dy;
    particle.theta_z = carmen_normalize_theta( particle.theta_z + rot_2 );
  }
}


// --------------------------------------------------------------------------
void CrossrangeDownrangeRotationMotionModel::
    propagateParticles( std::vector<Particle>& particles,
                        double translation,
                        double rotation,
                        double /*timeDifference*/,
                        unsigned int localizationStep )
// --------------------------------------------------------------------------
{
  VERBOSE_OUT << color::purple << "New odometry CROSSRANGE_DOWNRANGE_ROTATION: translation=" << translation << ", rotation=" << rotation << color::normal << endl;

  // Special noise for first, global localization steps
  double fd = params->config_localization_noise_dist_discount_factor * exp( - params->config_localization_noise_dist_discount_rate * localizationStep );
  double fr = params->config_localization_noise_rot_discount_factor  * exp( - params->config_localization_noise_rot_discount_rate  * localizationStep );

  double noisy_downrange, noisy_crossrange, noisy_rotation;
  double dx, dy, _cos, _sin, move_angle;

  double expected_downrange  = translation * params->downrangeMeanByTrans  + rotation * params->downrangeMeanByRot;
  double expected_crossrange = translation * params->crossrangeMeanByTrans + rotation * params->crossrangeMeanByRot;
  double expected_rotation   = translation * params->rotationMeanByTrans   + rotation * params->rotationMeanByRot;

  /** THIS IS WRONG: sum of std_devs a, b :  sum = sqrt( a^2 + b^2 ) !!!
   *
  double downrange_std_dev = fabs( translation * params->downrangeStdDevByTrans )
        + fabs( rotation * params->downrangeStdDevByRot )
        + fd;

  double crossrange_std_dev = fabs( translation * params->crossrangeStdDevByTrans )
        + fabs( rotation * params->crossrangeStdDevByRot );

  double rotation_std_dev = fabs( translation * params->rotationStdDevByTrans )
        + fabs( rotation    * params->rotationStdDevByRot )
        + fabs( translation * params->config_artificial_noise_rot_per_dist )
        + fabs( rotation    * params->config_artificial_noise_rot_per_rot )
        + fr;
  **/

  double downrange_std_dev = sqrt(   sqr( translation * params->downrangeStdDevByTrans )
                                   + sqr( rotation * params->downrangeStdDevByRot )
                                   + sqr(fd) );

  double crossrange_std_dev = sqrt(   sqr( translation * params->crossrangeStdDevByTrans )
                                    + sqr( rotation * params->crossrangeStdDevByRot ) );

  double rotation_std_dev = sqrt(   sqr( translation * params->rotationStdDevByTrans )
                                  + sqr( rotation    * params->rotationStdDevByRot )
                                  + sqr( translation * params->config_artificial_noise_rot_per_dist )
                                  + sqr( rotation    * params->config_artificial_noise_rot_per_rot )
                                  + sqr( fr ) );

  // Iterate over all localization particles
  for ( uint p = 0; p < particles.size(); ++p )
  {
    Particle& particle = particles[p];

    noisy_downrange  = expected_downrange  + m_pFastGaussRand->getRandomNumber() * downrange_std_dev;
    noisy_crossrange = expected_crossrange + m_pFastGaussRand->getRandomNumber() * crossrange_std_dev;
    noisy_rotation   = expected_rotation   + m_pFastGaussRand->getRandomNumber() * rotation_std_dev;

    move_angle = particle.theta_z + 0.5 * noisy_rotation;
    _cos = cos( move_angle );
    _sin = sin( move_angle );

    dx = _cos * noisy_downrange - _sin * noisy_crossrange;
    dy = _sin * noisy_downrange + _cos * noisy_crossrange;

    particle.x       += dx;
    particle.y       += dy;
    particle.theta_z = carmen_normalize_theta( particle.theta_z + noisy_rotation );
  }
}


// --------------------------------------------------------------------------
void CrossrangeDownrangeRotationMotionModel::
    learnParameters( const std::vector<std::pair<carmen_point_t, carmen_point_t> > motionMeasurements )
// --------------------------------------------------------------------------
{
  learnParametersClosedFormLeastSquares( motionMeasurements );
}


// --------------------------------------------------------------------------
void CrossrangeDownrangeRotationMotionModel::
    leastSquaresClosedForm( const double sumNormal[2][2],
                            const double sumY[2],
                            double leastSquaresSolution[2] )
// --------------------------------------------------------------------------
{
  leastSquaresSolution[0] = ((sumY[0]*sumNormal[1][1]) - (sumY[1]*sumNormal[0][1])) / ((sumNormal[0][0]*sumNormal[1][1]) - (sumNormal[0][1]*sumNormal[1][0]));
  leastSquaresSolution[1] = ((sumNormal[0][0]*sumY[1]) - (sumNormal[1][0]*sumY[0])) / ((sumNormal[0][0]*sumNormal[1][1]) - (sumNormal[0][1]*sumNormal[1][0]));
}


struct MotionSampleCDT {
  MotionSampleCDT() : translation(0), rotation(0), C(0), D(0), R(0) {}
  MotionSampleCDT( const MotionSampleCDT& other ) : translation(other.translation), rotation(other.rotation), C(other.C), D(other.D), R(other.R) {}
  MotionSampleCDT& operator=( const MotionSampleCDT& other ) {
    if( this != &other ) {
      this->translation  = other.translation;
      this->rotation     = other.rotation;
      this->C            = other.C;
      this->D            = other.D;
      this->R            = other.R;
    }
    return *this;
  }
  double translation;
  double rotation;
  double C;
  double D;
  double R;
};

// --------------------------------------------------------------------------
void CrossrangeDownrangeRotationMotionModel::
    learnParametersClosedFormLeastSquares( const std::vector<std::pair<carmen_point_t, carmen_point_t> > motionMeasurements )
// --------------------------------------------------------------------------
{
  std::vector< MotionSampleCDT > motionSamples;

  double tempLeastSquaresSolutions[2];
  double sumNormalC[2], sumNormalD[2], sumNormalR[2];
  double sumNormalTransRot[2][2];

  ////
  // gather sample data
  for( int i=0; i<motionMeasurements.size(); ++i ) {
    MotionSampleCDT curSample;

    const carmen_point_t& odometry     = motionMeasurements[i].first;
    const carmen_point_t& ground_truth = motionMeasurements[i].second;

    // transform ground truth to D, C, R
    const double _cos = cos( - 0.5 * ground_truth.theta ), _sin = sin( - 0.5 * ground_truth.theta );
    curSample.D = _cos * ground_truth.x - _sin * ground_truth.y;
    curSample.C = _sin * ground_truth.x + _cos * ground_truth.y;
    curSample.R = ground_truth.theta;

    // get translation and rotation from odometry reading
    curSample.translation = sqrt(odometry.x*odometry.x + odometry.y*odometry.y);
    curSample.rotation    = odometry.theta;

    motionSamples.push_back( curSample );
  }

  /////
  // compute means of C, D and T given data using least squares

  for( int i=0; i<2; ++i ) {
    sumNormalC[i] = 0;
    sumNormalD[i] = 0;
    sumNormalR[i] = 0;
    for( int j=0; j<2; ++j )
      sumNormalTransRot[i][j] = 0;
  }

  for( int i=0; i<motionSamples.size(); ++i ) {
    const MotionSampleCDT& s = motionSamples[i];

    sumNormalTransRot[ 0 ][ 0 ] += s.translation * s.translation;
    sumNormalTransRot[ 0 ][ 1 ] += s.translation * s.rotation;
    sumNormalTransRot[ 1 ][ 1 ] += s.rotation * s.rotation;

    sumNormalC[ 0 ] += s.C * s.translation;
    sumNormalC[ 1 ] += s.C * s.rotation;
    sumNormalD[ 0 ] += s.D * s.translation;
    sumNormalD[ 1 ] += s.D * s.rotation;
    sumNormalR[ 0 ] += s.R * s.translation;
    sumNormalR[ 1 ] += s.R * s.rotation;
  }
  sumNormalTransRot[ 1 ][ 0 ] = sumNormalTransRot[ 0 ][ 1 ];

  leastSquaresClosedForm( sumNormalTransRot, sumNormalD, tempLeastSquaresSolutions );
  params->downrangeMeanByTrans = tempLeastSquaresSolutions[0];
  params->downrangeMeanByRot   = tempLeastSquaresSolutions[1];
  leastSquaresClosedForm( sumNormalTransRot, sumNormalC, tempLeastSquaresSolutions );
  params->crossrangeMeanByTrans = tempLeastSquaresSolutions[0];
  params->crossrangeMeanByRot   = tempLeastSquaresSolutions[1];
  leastSquaresClosedForm( sumNormalTransRot, sumNormalR, tempLeastSquaresSolutions );
  params->rotationMeanByTrans = tempLeastSquaresSolutions[0];
  params->rotationMeanByRot   = tempLeastSquaresSolutions[1];


  /////
  // compute standard deviations of C, D and T given data and means using least squares

  for( int i=0; i<2; ++i ) {
    sumNormalC[i] = 0;
    sumNormalD[i] = 0;
    sumNormalR[i] = 0;
    for( int j=0; j<2; ++j )
      sumNormalTransRot[i][j] = 0;
  }

  for( int i=0; i<motionSamples.size(); ++i ) {
    const MotionSampleCDT& s = motionSamples[i];

    sumNormalTransRot[ 0 ][ 0 ] += sqr(s.translation) * sqr(s.translation);
    sumNormalTransRot[ 0 ][ 1 ] += sqr(s.translation) * sqr(s.rotation);
    sumNormalTransRot[ 1 ][ 1 ] += sqr(s.rotation)    * sqr(s.rotation);

    sumNormalC[ 0 ] += sqr(s.C - (params->crossrangeMeanByTrans * s.translation + params->crossrangeMeanByRot * s.rotation)) * sqr(s.translation);
    sumNormalC[ 1 ] += sqr(s.C - (params->crossrangeMeanByTrans * s.translation + params->crossrangeMeanByRot * s.rotation)) * sqr(s.rotation);
    sumNormalD[ 0 ] += sqr(s.D - (params->downrangeMeanByTrans  * s.translation + params->downrangeMeanByRot  * s.rotation)) * sqr(s.translation);
    sumNormalD[ 1 ] += sqr(s.D - (params->downrangeMeanByTrans  * s.translation + params->downrangeMeanByRot  * s.rotation)) * sqr(s.rotation);
    sumNormalR[ 0 ] += sqr(s.R - (params->rotationMeanByTrans   * s.translation + params->rotationMeanByRot   * s.rotation)) * sqr(s.translation);
    sumNormalR[ 1 ] += sqr(s.R - (params->rotationMeanByTrans   * s.translation + params->rotationMeanByRot   * s.rotation)) * sqr(s.rotation);
  }
  sumNormalTransRot[ 1 ][ 0 ] = sumNormalTransRot[ 0 ][ 1 ];

  leastSquaresClosedForm( sumNormalTransRot, sumNormalD, tempLeastSquaresSolutions );
  params->downrangeStdDevByTrans = (tempLeastSquaresSolutions[0] >= 0 ? sqrt( tempLeastSquaresSolutions[0] ) : 0);
  params->downrangeStdDevByRot   = (tempLeastSquaresSolutions[1] >= 0 ? sqrt( tempLeastSquaresSolutions[1] ) : 0);
  leastSquaresClosedForm( sumNormalTransRot, sumNormalC, tempLeastSquaresSolutions );
  params->crossrangeStdDevByTrans = (tempLeastSquaresSolutions[0] >= 0 ? sqrt( tempLeastSquaresSolutions[0] ) : 0);
  params->crossrangeStdDevByRot   = (tempLeastSquaresSolutions[1] >= 0 ? sqrt( tempLeastSquaresSolutions[1] ) : 0);
  leastSquaresClosedForm( sumNormalTransRot, sumNormalR, tempLeastSquaresSolutions );
  params->rotationStdDevByTrans = (tempLeastSquaresSolutions[0] >= 0 ? sqrt( tempLeastSquaresSolutions[0] ) : 0);
  params->rotationStdDevByRot   = (tempLeastSquaresSolutions[1] >= 0 ? sqrt( tempLeastSquaresSolutions[1] ) : 0);
}
