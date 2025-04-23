/** @addtogroup ambisense_slam libambisense_slam_interface **/
// @{

/** \file ambisense_slam_interface.h
 * \brief Definition of the interface of AmbiSense SLAM
 *
 * This file specifies the interface to subscribe the messages of
 * that module and to receive its data via IPC.
 **/

#ifndef SCIROCO_CARMEN__AMBISENSE_SLAM_INTERFACE_H
#define SCIROCO_CARMEN__AMBISENSE_SLAM_INTERFACE_H

#include <carmenx/ambisense_slam/ambisense_slam_messages.h>

#ifndef CARMEN_H
struct carmen_localize_globalpos_message;
#endif


#ifdef __cplusplus
extern "C" {
#endif

/** Compute the deviation of the two poses in meters */
float carmen_ambisense_slam_compute_deviation(
        carmen_ambisense_slam_globalpos_message * ambisense_globalpos_msg,
        carmen_localize_globalpos_message       * localize_globalpos_msg,
        carmen_base_odometry_message            * odometry_msg );


// Map message ////////////////////////////////////////////
void
carmen_ambisense_slam_subscribe_map_message( carmen_ambisense_slam_map_message *map_msg,
        carmen_handler_t handler,
        carmen_subscribe_t subscribe_how);

void
carmen_ambisense_slam_unsubscribe_map_message( carmen_handler_t handler );

void
carmen_ambisense_slam_define_map_message();

void
carmen_ambisense_slam_publish_map_message( carmen_ambisense_slam_map_message *msg );

/** Print the given message to stdout */
void
carmen_ambisense_slam_print_map_message( carmen_ambisense_slam_map_message *msg );


// Landmark message ///////////////////////////////////////
void
    carmen_ambisense_slam_subscribe_landmark_message( carmen_ambisense_slam_landmark_message *landmark_msg,
    carmen_handler_t handler,
    carmen_subscribe_t subscribe_how);

void
    carmen_ambisense_slam_unsubscribe_landmark_message( carmen_handler_t handler );

void
    carmen_ambisense_slam_define_landmark_message();

void
    carmen_ambisense_slam_publish_landmark_message( carmen_ambisense_slam_landmark_message *msg );

/** Send the landmark coordinates with uncorrelated
 *  covariance values in each of the 6 dimensions
 *  @see carmen_ambisense_slam_send_landmark_with_full_covariance_matrix
 */
void
    carmen_ambisense_slam_send_landmark( carmen_ambisense_slam_landmark_type_t type,
                                         const char*        id,
                                         carmen_6d_point_t* position,
                                         carmen_6d_point_t* variance );

/** Send the landmark coordinates with the entire matrix of
 *  covariance values
 *  @see carmen_ambisense_slam_send_landmark
 */
void
    carmen_ambisense_slam_send_landmark_with_full_covariance_matrix(
        carmen_ambisense_slam_landmark_type_t type,
        const char*        name,
        carmen_6d_point_t* position,
        double             variance[36] );

/** Print the given message to stdout */
void
    carmen_ambisense_slam_print_landmark_message( carmen_ambisense_slam_landmark_message *msg );


// Landmark relocation message ////////////////////////////////////////////
void
    carmen_ambisense_slam_subscribe_landmark_relocation_message( carmen_ambisense_slam_landmark_relocation_message *landmark_relocation_msg,
        carmen_handler_t handler,
        carmen_subscribe_t subscribe_how);

void
    carmen_ambisense_slam_unsubscribe_landmark_relocation_message( carmen_handler_t handler );

void
    carmen_ambisense_slam_define_landmark_relocation_message();

void
    carmen_ambisense_slam_publish_landmark_relocation_message( carmen_ambisense_slam_landmark_relocation_message *msg );

/** Publish the probability that the given tag is relocated
 *  by the given distance
 *  @param relocation_probability The relocation probability
 */
void
    carmen_ambisense_slam_publish_landmark_relocation( const char* tag_id, double relocation_probability, double relocation_distance );

void
    carmen_ambisense_slam_print_landmark_relocation_message( carmen_ambisense_slam_landmark_relocation_message* msg );


// Map request message ////////////////////////////////////
void
carmen_ambisense_slam_subscribe_map_request_message(
        carmen_ambisense_slam_map_request_message *map_msg,
        carmen_handler_t handler,
        carmen_subscribe_t subscribe_how);

void
carmen_ambisense_slam_unsubscribe_map_request_message( carmen_handler_t handler );

void
carmen_ambisense_slam_define_map_request_message();

void
carmen_ambisense_slam_publish_map_request_message(
        carmen_ambisense_slam_map_request_message *msg );


// Particle message ///////////////////////////////////////
void
carmen_ambisense_slam_subscribe_ambisense_particles_message(carmen_ambisense_particles_message *ambisense_slam,
        carmen_handler_t handler,
        carmen_subscribe_t subscribe_how);

void
carmen_ambisense_slam_unsubscribe_ambisense_particles_message(carmen_handler_t handler);

void
carmen_ambisense_slam_define_ambisense_particles_message();

void
carmen_ambisense_slam_publish_ambisense_particles_message(carmen_ambisense_particles_message* msg);

void
carmen_ambisense_slam_print_ambisense_particles_message( carmen_ambisense_particles_message *tag_particles );


// Particle filter control message ////////////////////////
void
carmen_ambisense_slam_define_ambisense_pf_control_message();

void
carmen_ambisense_slam_subscribe_pf_control_message(
        carmen_ambisense_slam_particle_filter_control_message *pf_control,
        carmen_handler_t handler,
        carmen_subscribe_t subscribe_how );

void
carmen_ambisense_slam_unsubscribe_pf_control_message(carmen_handler_t handler);

void
carmen_ambisense_slam_publish_ambisense_pf_control_message(
    carmen_ambisense_slam_particle_filter_control_message* msg );


// Particle mapping control message ////////////////////////
void
carmen_ambisense_slam_define_ambisense_mapping_control_message();

void
carmen_ambisense_slam_subscribe_mapping_control_message(
    carmen_ambisense_slam_mapping_control_message *mapping_control,
        carmen_handler_t handler,
        carmen_subscribe_t subscribe_how );

void
carmen_ambisense_slam_unsubscribe_mapping_control_message(carmen_handler_t handler);

int
carmen_ambisense_slam_publish_ambisense_mapping_control_message(
    carmen_ambisense_slam_mapping_control_message* msg );


// Global pose message ///////////////////////////////////////
void
carmen_ambisense_slam_subscribe_globalpos_message(carmen_ambisense_slam_globalpos_message *msg,
    carmen_handler_t handler,
    carmen_subscribe_t subscribe_how);

void
carmen_ambisense_slam_unsubscribe_globalpos_message(carmen_handler_t handler);

void
carmen_ambisense_slam_define_globalpos_message();

void
carmen_ambisense_slam_publish_globalpos_message(carmen_ambisense_slam_globalpos_message* msg);

/** Extrapolate the given global position by the odometry accumulated meanwhile */
void
carmen_ambisense_slam_extrapolate_globalpos( carmen_ambisense_slam_globalpos_message* globalpos,
                                             carmen_base_odometry_message*            odometry );

/** Extrapolate the given laser-computed global position by the odometry accumulated meanwhile */
void
carmen_ambisense_slam_extrapolate_localize_globalpos(
        carmen_localize_globalpos_message * globalpos,
        carmen_base_odometry_message      * odometry );

/** Copy the globalpos message src to the globalpos message dest */
void
carmen_ambisense_slam_copy_globalpos_message(carmen_ambisense_slam_globalpos_message *src,
                                             carmen_ambisense_slam_globalpos_message *dest);

#ifdef __cplusplus
}
#endif

#endif // SCIROCO_CARMEN__AMBISENSE_SLAM_INTERFACE_H
// @}
