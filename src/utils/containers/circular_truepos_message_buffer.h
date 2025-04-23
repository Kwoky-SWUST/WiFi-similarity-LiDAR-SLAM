#ifndef CIRCULAR_TRUEPOS_MESSAGE_BUFFER_49839544_H
#define CIRCULAR_TRUEPOS_MESSAGE_BUFFER_49839544_H

#include <carmen/global.h>
#include <carmen/simulator_messages.h>

#include "circular_message_buffer.hpp"

/** Specialized version of CircularMessageBuffer for truepos messages
 *  provides a method to get an interpolated odometry as well truepos (ground truth) poses from the truepos message buffer
 *  @author Artur Koch
 */
class CircularTrueposMessageBuffer : public CircularMessageBuffer< carmen_simulator_truepos_message > {
  public:
    CircularTrueposMessageBuffer( const bool appendBack = true );
    CircularTrueposMessageBuffer( size_type max_size,
                                  const bool appendBack = true,
                                  DeepCopyFunctionType fptr_deepCopy = &CircularMessageBuffer< carmen_simulator_truepos_message >::defaultDeepCopy,
                                  DestructorFunctionType fptr_destructor = &CircularMessageBuffer< carmen_simulator_truepos_message >::defaultDestructor );

    /** Compute interpolated ground truth pose for the given timestamp
     * @param timestamp the timestamp of the desired truepos value
     * @param interpolated_truepos outputs the interpolated truepos (ground truth) pose
     * @return
     */
    bool getInterpolatedTruepos( const double timestamp, carmen_point_t& interpolated_truepos ) const;

    /** Compute interpolated odometric pose for the given timestamp in the truepos message buffer
     * @param timestamp the timestamp of the desired odometry value from truepos messages
     * @param interpolated_odometry outputs the interpolated odometric pose
     * @return
     */
    bool getInterpolatedOdometryPose( const double timestamp, carmen_point_t& interpolated_odometry ) const;

};


#endif // CIRCULAR_TRUEPOS_MESSAGE_BUFFER_49839544_H
