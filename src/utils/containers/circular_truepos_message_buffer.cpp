#include <carmen/carmen.h>
#include <utils/geometry/point_utils.h>

#include "circular_truepos_message_buffer.h"


// --------------------------------------------------------------------------
CircularTrueposMessageBuffer::
CircularTrueposMessageBuffer( const bool appendBack )
// --------------------------------------------------------------------------
  : CircularMessageBuffer< carmen_simulator_truepos_message >( appendBack )
{
}

// --------------------------------------------------------------------------
CircularTrueposMessageBuffer::
CircularTrueposMessageBuffer( size_type max_size,
                              const bool appendBack,
                              DeepCopyFunctionType fptr_deepCopy,
                              DestructorFunctionType fptr_destructor )
// --------------------------------------------------------------------------
  : CircularMessageBuffer< carmen_simulator_truepos_message >( max_size, appendBack, fptr_deepCopy, fptr_destructor )
{
}

// --------------------------------------------------------------------------
bool CircularTrueposMessageBuffer::
getInterpolatedTruepos( const double timestamp, carmen_point_t& interpolated_truepos ) const
// --------------------------------------------------------------------------
{
  size_type lower_index, upper_index;
  double fraction;

  // compute nearest neighbor indices
  const bool bSuccess = findNNIndicesOfTimestamp( timestamp, lower_index, upper_index, &fraction );

  if( !bSuccess )
    return false;

  // compute the actual odometric pose
  const carmen_point_t& infimum  = m_data[ lower_index ].truepose;
  const carmen_point_t& supremum = m_data[ upper_index ].truepose;
  interpolated_truepos.x         = infimum.x + fraction * ( supremum.x - infimum.x );
  interpolated_truepos.y         = infimum.y + fraction * ( supremum.y - infimum.y );
  interpolated_truepos.theta_z   = interpolate_heading( infimum.theta, supremum.theta, fraction );
  return true;
}

// --------------------------------------------------------------------------
bool CircularTrueposMessageBuffer::
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
  const carmen_point_t& infimum  = m_data[ lower_index ].odometrypose;
  const carmen_point_t& supremum = m_data[ upper_index ].odometrypose;
  interpolated_odometry.x        = infimum.x + fraction * ( supremum.x - infimum.x );
  interpolated_odometry.y        = infimum.y + fraction * ( supremum.y - infimum.y );
  interpolated_odometry.theta_z  = interpolate_heading( infimum.theta, supremum.theta, fraction );
  return true;
}
