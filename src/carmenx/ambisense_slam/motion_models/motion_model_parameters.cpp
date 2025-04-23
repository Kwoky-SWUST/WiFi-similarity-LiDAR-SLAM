#include <carmen/carmen.h>
#include <motion_models/motion_model_parameters.h>
#include <utils/quality/sciroco_exception.h>
#include <utils/text/to_string.h>

using namespace std;


// --------------------------------------------------------------------------
MotionModelParameters::
    MotionModelParameters()
// --------------------------------------------------------------------------
{
  config_localization_gaussian_translational_noise = 0.0;
  config_localization_gaussian_rotational_noise    = 0.0;

  rtrTranslationMean         = 1.0;
  rtrRotationMean            = 1.0;
  rtrStdDevTranslationPerTranslation = 0.1;
  config_localization_noise_dist_per_rot  = 0.03;
  rtrStdDevRotationPerRotation   = 10.0 / 180.0*M_PI;
  config_localization_noise_rot_per_dist  =  2.0 / 180.0*M_PI;

  config_artificial_noise_dist_per_dist = 0.0;
  config_artificial_noise_rot_per_rot   = 0.0;
  config_artificial_noise_rot_per_dist  = 0.0;
  config_artificial_noise_dist_per_rot  = 0.0;

  config_localization_noise_rot_discount_factor  = 0.0;
  config_localization_noise_rot_discount_rate    = 0.0;
  config_localization_noise_dist_discount_factor = 0.0;
  config_localization_noise_dist_discount_rate   = 0.0;

  crossrangeMeanByTrans   = 0.0;
  crossrangeMeanByRot     = 0.0;
  crossrangeStdDevByTrans = 0.0;
  crossrangeStdDevByRot   = 0.0;

  downrangeMeanByTrans    = 0.0;
  downrangeMeanByRot      = 0.0;
  downrangeStdDevByTrans  = 0.0;
  downrangeStdDevByRot    = 0.0;

  rotationMeanByTrans     = 0.0;
  rotationMeanByRot       = 0.0;
  rotationStdDevByTrans   = 0.0;
  rotationStdDevByRot     = 0.0;

  for ( int dim1 = 0; dim1 < 3; ++dim1 )
  {
    rigidMean[dim1] = 0.0;
    for ( int dim2 = 0; dim2 < 3; ++dim2 )
      rigidStdDev[dim1][dim2] = 0.0;
  }

}


// --------------------------------------------------------------------------
void MotionModelParameters::
setRigidBodyParameters( char* means, char* covariances )
// --------------------------------------------------------------------------
{
  ScirocoException::test( means != NULL, "ambisense_motion_model_rigid_body_means is NULL", __FUNCTION__, __LINE__ );
  ScirocoException::test( covariances != NULL, "ambisense_motion_model_rigid_body_covariances is NULL", __FUNCTION__, __LINE__ );

  char delimiters[] = " ";

  // Tokenize means
  char* result = strtok( means, delimiters );
  int   dim1 = 0;
  while ( result != NULL )
  {
    if ( dim1 < 3 )
      rigidMean[dim1] = strtod( result, NULL );
    ++dim1;
    result = strtok( NULL, delimiters );
  }
  if ( dim1 > 3 )
    throw ScirocoException( "ERROR: Too many (>3) parameters in parameter ambisense_motion_model_rigid_body_means: " + string(means), ScirocoException::INVALID_ARGUMENT );

  // Tokenize means
  result = strtok( covariances, delimiters );
  int   dim2 = 0;
  double value;
  dim1 = 0;
  while ( result != NULL )
  {
    if ( dim1 < 3 && dim2 < 3 )
    {
      value = strtod( result, NULL );
      ScirocoException::test( ( !isnan(value) && ! isinf(value) && value > 0 ), "Invalid parameter value in ambisense_motion_model_rigid_body_covariances: " + toString(value), __FUNCTION__, __LINE__ );

      rigidStdDev[dim1][dim2] = sqrt( value );
    }
    ++dim2;
    if ( dim2 == 3 )
    {
      dim2 = 0;
      ++dim1;
    }
    result = strtok( NULL, delimiters );
  }
  if ( dim1 > 3 )
    throw ScirocoException( "ERROR: Too many (>9) parameters in parameter ambisense_motion_model_rigid_body_covariances: " + string(means), ScirocoException::INVALID_ARGUMENT );

}


// --------------------------------------------------------------------------
void MotionModelParameters::
    disableGlobalLocalizationNoise()
// --------------------------------------------------------------------------
{
  config_localization_noise_rot_discount_factor  = 0.0;
  config_localization_noise_rot_discount_rate    = 0.0;
  config_localization_noise_dist_discount_factor = 0.0;
  config_localization_noise_dist_discount_rate   = 0.0;
}


// --------------------------------------------------------------------------
void MotionModelParameters::
    print( std::string indentation, std::ostream & ostr )
// --------------------------------------------------------------------------
{
#define IND ostr << indentation
  IND << "Noise modeling:\n";
  IND << "  Continuous translational Gaussian noise:    " << config_localization_gaussian_translational_noise << '\n';
  IND << "  Continuous rotational Gaussian noise:       " << config_localization_gaussian_rotational_noise << '\n';
  IND << "  Translational Gaussian noise per distance:  " << rtrStdDevTranslationPerTranslation << '\n';
  IND << "  Rotational Gaussian noise per rotation:     " << rtrStdDevRotationPerRotation << '\n';
  IND << "  Rotational Gaussian noise per translation:  " << config_localization_noise_rot_per_dist << '\n';

  IND << "  Downrange/crossrange/rotation model:\n";
  IND << "    Downrange:  Mean by trans/rot:            " << downrangeMeanByTrans << "/" << downrangeMeanByRot << '\n';
  IND << "                Std.dev. trans/rot:           " << downrangeStdDevByTrans << "/" << downrangeStdDevByRot << '\n';
  IND << "    Crossrange: Mean by trans/rot:            " << crossrangeMeanByTrans << "/" << crossrangeMeanByRot << '\n';
  IND << "                Std.dev. trans/rot:           " << crossrangeStdDevByTrans << "/" << crossrangeStdDevByRot << '\n';
  IND << "    Rotation:   Mean by trans/rot:            " << rotationMeanByTrans << "/" << rotationMeanByRot << '\n';
  IND << "                Std.dev. trans/rot:           " << rotationStdDevByTrans << "/" << rotationStdDevByRot << '\n';

  IND << "  Rigid-body model:\n";
  IND << "    Means (F/S/R):                            " << rigidMean[0] << ", " << rigidMean[1] << ", " << rigidMean[2] << '\n';
  IND << "    Covariances: FF FS FR:                    " << rigidStdDev[0][0] << ", " << rigidStdDev[0][1] << ", " << rigidStdDev[0][2] << '\n';
  IND << "                 SF SS SR:                    " << rigidStdDev[1][0] << ", " << rigidStdDev[1][1] << ", " << rigidStdDev[1][2] << '\n';
  IND << "                 RF RS RR:                    " << rigidStdDev[2][0] << ", " << rigidStdDev[2][1] << ", " << rigidStdDev[2][2] << '\n';

  IND << "Artificial noise:\n";
  IND << "  Transl. noise per translation:              " << config_artificial_noise_dist_per_dist << '\n';
  IND << "  Transl. noise per rotation:                 " << config_artificial_noise_dist_per_rot << '\n';
  IND << "  Rot. noise per translation:                 " << config_artificial_noise_rot_per_dist << '\n';
  IND << "  Rot. noise per rotation:                    " << config_artificial_noise_rot_per_rot << '\n';

}
