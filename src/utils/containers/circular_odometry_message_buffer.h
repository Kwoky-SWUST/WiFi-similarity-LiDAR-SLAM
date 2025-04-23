#ifndef CIRCULAR_ODOMETRY_MESSAGE_BUFFER_478359834_H
#define CIRCULAR_ODOMETRY_MESSAGE_BUFFER_478359834_H

#include <carmen/global.h>
#include <carmen/base_messages.h>

#include "circular_message_buffer.hpp"

/** Specialized version of CircularMessageBuffer for odometry messages
 *  provides a method to get an interpolated odometry pose
 *  @author Artur Koch
 */
class CircularOdometryMessageBuffer : public CircularMessageBuffer< carmen_base_odometry_message > {
  public:
    CircularOdometryMessageBuffer( const bool appendBack = true );
    CircularOdometryMessageBuffer( size_type max_size,
                                   const bool appendBack = true,
                                   DeepCopyFunctionType fptr_deepCopy = &CircularMessageBuffer< carmen_base_odometry_message >::defaultDeepCopy,
                                   DestructorFunctionType fptr_destructor = &CircularMessageBuffer< carmen_base_odometry_message >::defaultDestructor );

    /** Compute interpolated odometric pose for the given timestamp
     * @param timestamp the timestamp of the desired odometry value
     * @param interpolated_odometry outputs the interpolated odometric pose
     * @return
     */
    bool getInterpolatedOdometryPose( const double timestamp, carmen_point_t& interpolated_odometry ) const;

};


#endif // CIRCULAR_ODOMETRY_MESSAGE_BUFFER_478359834_H
