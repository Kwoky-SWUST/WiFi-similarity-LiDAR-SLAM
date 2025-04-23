#ifndef SCIROCO_CARMEN__AMBISENSE_SLAM__MOTION_MODEL_PARAMETERS
#define SCIROCO_CARMEN__AMBISENSE_SLAM__MOTION_MODEL_PARAMETERS

#include <iostream>
#include <string>


/** Parameters of the various motion models
 *  @author Philipp Vorst
 */
// --------------------------------------------------------------------------
class MotionModelParameters
// --------------------------------------------------------------------------
{
  public:
    /** Constructor */
    MotionModelParameters();

    /** Write the parameters to the given stream */
    void print( std::string indentation, std::ostream & ostr );

    /** Set the variables for increased noise during the first localization
     *  steps -- for global localization -- zero.
     */
    void disableGlobalLocalizationNoise();

    /** Extract the parameters of the rigid body motion model from the
     *  given parameters.
     */
    void setRigidBodyParameters( char* means, char* covariances );

    double crossrangeMeanByTrans;
    double crossrangeMeanByRot;
    double crossrangeStdDevByTrans;
    double crossrangeStdDevByRot;

    double downrangeMeanByTrans;
    double downrangeMeanByRot;
    double downrangeStdDevByTrans;
    double downrangeStdDevByRot;

    double rotationMeanByTrans;
    double rotationMeanByRot;
    double rotationStdDevByTrans;
    double rotationStdDevByRot;

    /// Noise means of rigid body transformation
    double rigidMean[3];

    /// Square roots of noise covariances of rigid body transformation
    double rigidStdDev[3][3];


    /// The standard deviation of Gaussian noise, which is added to the particles
    /// after each cycle
    double config_localization_gaussian_translational_noise;

    /// The standard deviation of Gaussian noise, which is added to the particles
    /// after each cycle
    double config_localization_gaussian_rotational_noise;

    /// The systematic error in translation, represented by a factor by which
    /// the measured translation is multiplied
    double rtrTranslationMean;

    /// The systematic error in rotation, represented by a factor by which
    /// the measured rotation is multiplied
    double rtrRotationMean;

    /// The standard deviation of Gaussian noise which is added to the particles
    /// after a movement
    double rtrStdDevTranslationPerTranslation;

    /// The standard deviation of Gaussian noise which is added to the particles
    /// after a rotation
    double rtrStdDevRotationPerRotation;

    /// The standard deviation of Gaussian noise in rotation which is added to
    /// the particles after a translation
    double config_localization_noise_rot_per_dist;

    /// The standard deviation of Gaussian noise in translation which is added to
    /// the particles after a rotation
    double config_localization_noise_dist_per_rot;


    double config_localization_noise_rot_discount_factor;
    double config_localization_noise_rot_discount_rate;
    double config_localization_noise_dist_discount_factor;
    double config_localization_noise_dist_discount_rate;

    /// The standard deviation of Gaussian noise which is artificially added
    /// corresponding to a translation
    double config_artificial_noise_dist_per_dist;

    /// The standard deviation of Gaussian noise which is added to the particles
    /// after a rotation
    double config_artificial_noise_rot_per_rot;

    /// The standard deviation of Gaussian noise in rotation which is added to
    /// the particles after a translation
    double config_artificial_noise_rot_per_dist;

    /// The standard deviation of Gaussian noise in translation which is added to
    /// the particles after a rotation
    double config_artificial_noise_dist_per_rot;

};


#endif // SCIROCO_CARMEN__AMBISENSE_SLAM__MOTION_MODEL_PARAMETERS
