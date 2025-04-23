/** @addtogroup ambisense_slam libambisense_slam_interface**/
// @{

/** @file ambisense_slam_messages.h
 *  @brief Definition of the messages for the module AMBISENSE_SLAM.
 *
 *  This file specifies the messages for this modules used to transmit
 *  data via IPC to other modules.
 **/

#ifndef CROCO_CARMEN_AMBISENSE_SLAM_MESSAGES_H
#define CROCO_CARMEN_AMBISENSE_SLAM_MESSAGES_H

#include <utils/geometry/point_extension.h>


#ifdef __cplusplus
extern "C" {
#endif


/// For heartbeat messages etc.
#define CARMEN_AMBISENSE_SLAM_SERVER_MODULE_NAME "ambisense_slam_server"
#define CARMEN_AMBISENSE_MAP_SERVER_MODULE_NAME  "ambisense_map_server"

typedef struct {
  float x, y, z, theta, weight, accumulated_weight, log_weight;
} carmen_ambisense_particle_ipc_t;


/** The AMBISENSE_SLAM data message of the AMBISENSE_SLAM module **/
typedef struct {
  int                               num_particles;
  carmen_ambisense_particle_ipc_t  *particles;

  char                             *filter_id;

  carmen_3d_point_t                 center;
  carmen_3d_point_t                 std_deviation;
  double                            xy_covariance;
  int                               effective_sample_set_size;

  double                            timestamp;
  char                             *host;
} carmen_ambisense_particles_message;

#define  CARMEN_AMBISENSE_PARTICLES_NAME "ambisense_particles"
#define  CARMEN_AMBISENSE_PARTICLES_FMT  "{int,<{float,float,float,float,float}:1>,string,{double,double,double,double},{double,double,double,double},double,int,double,string}"


/** The types of commands which are understood and which can be realized via
 *  carmen_ambisense_slam_particle_filter_control_message.
 */
typedef enum
{
  AMBISENSE_PF_RESET          = 0,
  AMBISENSE_PF_SEND_PARTICLES = 1
} carmen_ambisense_pf_command;

typedef struct {
  carmen_ambisense_pf_command       command;
  int                               send_particles_of_all_filters;
  int                               num_filters_to_be_sent;
  char                            **filters_to_be_sent;

  double                            timestamp;
  char                             *host;
} carmen_ambisense_slam_particle_filter_control_message;

#define  CARMEN_AMBISENSE_SLAM_PF_CONTROL_NAME            "ambisense_slam_pf_control"
#define  CARMEN_AMBISENSE_SLAM_PF_CONTROL_FMT             "{int,int,int,<string:2>,double,string}"



/** The types of commands which are understood and which can be realized via
 *  carmen_ambisense_slam_mapping_control_message.
 */
typedef enum
{
  AMBISENSE_MAPPING_RESTART = 0
} carmen_ambisense_mapping_command;

typedef enum
{
  AMBISENSE_SENSOR_NODE_TYPE_RFID      = 0,
  AMBISENSE_SENSOR_NODE_TYPE_BLUETOOTH = 1,
  AMBISENSE_SENSOR_NODE_TYPE_WLAN      = 2
} carmen_sensor_node_type;

typedef struct {
  carmen_ambisense_mapping_command  command;
  carmen_sensor_node_type           node_type;
  double                            timestamp;
  char                             *host;
} carmen_ambisense_slam_mapping_control_message;

#define  CARMEN_AMBISENSE_SLAM_MAPPING_CONTROL_NAME            "ambisense_slam_mapping_control"
#define  CARMEN_AMBISENSE_SLAM_MAPPING_CONTROL_FMT             "{int,int,double,string}"



typedef struct {
  carmen_point_t globalpos, globalpos_std;
  carmen_point_t odometrypos;
  double globalpos_xy_cov;
  int converged;
  /** This identifier can be used to publish/retrieve several pose estimates,
   *  e.g. obtained via different localization methods and for fusing them */
  char *pose_estimate_id;
  double timestamp;
  char *host;
} carmen_ambisense_slam_globalpos_message;

#define CARMEN_AMBISENSE_SLAM_GLOBALPOS_NAME "carmen_ambisense_slam_globalpos"
#define CARMEN_AMBISENSE_SLAM_GLOBALPOS_FMT  "{{double,double,double},{double,double,double},{double,double,double},double,int,string,double,string}"


/** Enumeration of possible landmark types used in maps
 *  @see carmen_ambisense_slam_map_message
 */
typedef enum {
  LANDMARK_TYPE_UNKNOWN = 0,
  LANDMARK_TYPE_RFID    = 1
} carmen_ambisense_slam_landmark_type_t;


/** Landmark object
 *  @see carmen_ambisense_slam_map_message
 */
typedef struct {
  carmen_ambisense_slam_landmark_type_t   type;
  char                                   *name;
  char                                   *description;
  carmen_6d_point_t                       position;
  carmen_6d_point_t                       position_variance;
} carmen_ambisense_slam_landmark_t;


/** Message which defines a map of ambient landmarks.
 *  @see carmen_ambisense_slam_landmark_t
 */
typedef struct {
  int                                num_landmarks;
  carmen_ambisense_slam_landmark_t  *landmarks;

  char                              *map_name;
  double                             timestamp;
  char                              *host;
} carmen_ambisense_slam_map_message;

#define  CARMEN_AMBISENSE_SLAM_MAP_MESSAGE_NAME            "ambisense_slam_map_message"
#define  CARMEN_AMBISENSE_SLAM_MAP_MESSAGE_FMT             "{int,<{int,string,string,{double,double,double,double,double,double},{double,double,double,double,double,double}}:1>,string,double,string}"


/** Message which defines a landmark position */
typedef struct {
  carmen_ambisense_slam_landmark_type_t  type;
  char*                                  name;
  carmen_6d_point_t                      position;
//   carmen_6d_point_t                      position_variance;
  double                                 position_covariance[36];

  double                                 timestamp;
  char                                  *host;
} carmen_ambisense_slam_landmark_message;

#define  CARMEN_AMBISENSE_SLAM_LANDMARK_MESSAGE_NAME            "ambisense_slam_landmark_message"
#define  CARMEN_AMBISENSE_SLAM_LANDMARK_MESSAGE_FMT             "{int,string,{double,double,double,double,double,double},[double:36],double,string}"
// #define  CARMEN_AMBISENSE_SLAM_LANDMARK_MESSAGE_FMT             "{int,string,{double,double,double,double,double,double},{double,double,double,double,double,double},double,string}"


/** Message which defines information about a relocalized landmark (e.g., tag) */
typedef struct {
  carmen_ambisense_slam_landmark_type_t  type;
  char*   name;
  int     num_predictions;
  int*    measurement_horizon;
  double* predicted_distances;
  double* prediction_likelihoods;
  double* likelihood_not_relocated;

  double  timestamp;
  char*   host;
} carmen_ambisense_slam_landmark_relocation_message;

#define  CARMEN_AMBISENSE_SLAM_LANDMARK_RELOCATION_MESSAGE_NAME            "ambisense_slam_landmark_relocation_message"
#define  CARMEN_AMBISENSE_SLAM_LANDMARK_RELOCATION_MESSAGE_FMT             "{int,string,int,<int:3>,<double:3>,<double:3>,<double:3>,double,string}"


/** Message for requesting a map (list of landmarks of a given type)
 *  @param landmark_type The type of landmark for which the current map is
 *                       desired.
 *  @see   carmen_ambisense_slam_landmark_t
 */
typedef struct {
  carmen_ambisense_slam_landmark_t   landmark_type;
  double                             timestamp;
  char                              *host;
} carmen_ambisense_slam_map_request_message;

#define  CARMEN_AMBISENSE_SLAM_MAP_REQUEST_MESSSAGE_NAME            "ambisense_slam_map_message"
#define  CARMEN_AMBISENSE_SLAM_MAP_REQUEST_MESSSAGE_FMT             "{int,double,string}"


#ifdef __cplusplus
}
#endif

#endif

// @}
