#ifndef SCIROCO_CARMEN__AMBISENSE_SLAM__MOTION_MODEL
#define SCIROCO_CARMEN__AMBISENSE_SLAM__MOTION_MODEL

#include <vector>
#include <ambisense_slam_config.h>
#include <motion_models/motion_model_parameters.h>


/** Base class for all motion models
 *  @author Philipp Vorst
 */
// --------------------------------------------------------------------------
class MotionModel
// --------------------------------------------------------------------------
{
  public:
    /** Constructor */
    MotionModel( MotionModelParameters* mmParams,
                 std::string            type );

    /** Destructor */
    virtual ~MotionModel() {};

    /** Write motion model info to the given stream */
    void print( std::string indentation, std::ostream & ostr );

    /** Apply the given motion to the sample set
     */
    virtual void apply( carmen_point_t         odometry,
                        double                 timeDifference,
                        unsigned int           localizationStep,
                        std::vector<Particle>& particles ) = 0;

    /** Return the probability of the given transformation, given
     *  the measured odometry reading.
     */
    virtual double getProbability( const carmen_point_t& transformation,
                                   const carmen_point_t& odometry ) const;

    /** Return the type of the model.
     *  @see m_sType
     */
    std::string getType() const;

    /** Return a pointer to the parameters of this model.
     */
    const MotionModelParameters* getParameters() const { return params; }

    /** Learn motion model parameters from given odometry and ground truth data
     * @param motionMeasurements vector of pairs of < odometry, ground_truth > samples used for learning
     */
    virtual void learnParameters( const std::vector< std::pair< carmen_point_t, carmen_point_t > > motionMeasurements ) {
      std::cout << "learnParameters not implemented for " << m_sType << " model, skipping!" << std::endl;
    };

  protected:
    /** Parameters of the motion model */
    MotionModelParameters* params;

    /** Type of motion model */
    std::string m_sType;
};


#endif // SCIROCO_CARMEN__AMBISENSE_SLAM__MOTION_MODEL
