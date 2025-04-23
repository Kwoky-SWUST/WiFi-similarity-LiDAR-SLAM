#include <carmen/carmen.h>
#include <utils/geometry/point_utils.h>

#include "circular_odometry_message_buffer.h"

// --------------------------------------------------------------------------
CircularOdometryMessageBuffer::
CircularOdometryMessageBuffer( const bool appendBack )
// --------------------------------------------------------------------------
  : CircularMessageBuffer< carmen_base_odometry_message >( appendBack )
{
}

// --------------------------------------------------------------------------
CircularOdometryMessageBuffer::CircularOdometryMessageBuffer( size_type max_size, const bool appendBack, DeepCopyFunctionType fptr_deepCopy,
                                                              DestructorFunctionType fptr_destructor )
// --------------------------------------------------------------------------
  : CircularMessageBuffer< carmen_base_odometry_message >( max_size, appendBack, fptr_deepCopy, fptr_destructor )
{
}


// --------------------------------------------------------------------------
bool CircularOdometryMessageBuffer::
getInterpolatedOdometryPose( const double timestamp, carmen_point_t& interpolated_odometry ) const
// --------------------------------------------------------------------------
{
  size_type lower_index, upper_index;
  double fraction;

  // compute nearest neighbor indices
  const bool bSuccess = findNNIndicesOfTimestamp( timestamp, lower_index, upper_index, &fraction );

  if( !bSuccess )
    return false;

  // compute the actual odometric pose
  const carmen_base_odometry_message& infimum  = m_data[ lower_index ];
  const carmen_base_odometry_message& supremum = m_data[ upper_index ];
  interpolated_odometry.x       = infimum.x + fraction * ( supremum.x - infimum.x );
  interpolated_odometry.y       = infimum.y + fraction * ( supremum.y - infimum.y );
  interpolated_odometry.theta_z = interpolate_heading( infimum.theta, supremum.theta, fraction );
  return true;
}
