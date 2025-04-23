#include <carmen/carmen.h>
#include "coordinate_transformation.h"
//
#include <fstream>
#include <iostream>
#include <iomanip>
// iostream+iomanip must be included before newmatio.h:
#include <newmat/newmatio.h>

using namespace std;


// --------------------------------------------------------------------------
Matrix HomogeneousTranslation( double x, double y, double z )
// --------------------------------------------------------------------------
{
  Matrix M(4,4);
  M = 0.0;
  M(1,1) = M(2,2) = M(3,3) = M(4,4) = 1;
  M(1,4) = x;
  M(2,4) = y;
  M(3,4) = z;

  return M;
}


// --------------------------------------------------------------------------
Matrix HomogeneousRollPitchYaw( double thetaX, double thetaY, double thetaZ )
// --------------------------------------------------------------------------
{
  Matrix M(4,4);
  M = 0.0;
  M(4,4) = 1;

  M(1,1) = cos( thetaZ ) * cos( thetaY );
  M(1,2) = cos( thetaZ ) * sin( thetaY ) * sin( thetaX ) - sin( thetaZ ) * cos( thetaX );
  M(1,3) = cos( thetaZ ) * sin( thetaY ) * cos( thetaX ) + sin( thetaZ ) * sin( thetaX );

  M(2,1) = sin( thetaZ ) * cos( thetaY );
  M(2,2) = sin( thetaZ ) * sin( thetaY ) * sin( thetaX ) + cos( thetaZ ) * cos( thetaX );
  M(2,3) = sin( thetaZ ) * sin( thetaY ) * cos( thetaX ) - cos( thetaZ ) * sin( thetaX );

  M(3,1) = - sin( thetaY );
  M(3,2) = cos( thetaY ) * sin( thetaX );
  M(3,3) = cos( thetaY ) * cos( thetaX );

//   cout << "RPY(" << carmen_radians_to_degrees( thetaX ) << ","
//       << carmen_radians_to_degrees( thetaY ) << ","
//       << carmen_radians_to_degrees( thetaZ ) << "):\n" << M << endl;

  return M;
}


// --------------------------------------------------------------------------
Matrix HomogeneousRollPitchYaw( const carmen_6d_point_t& p )
// --------------------------------------------------------------------------
{
  return HomogeneousRollPitchYaw(p.theta_x,p.theta_y,p.theta_z);
}


// --------------------------------------------------------------------------
Matrix HomogeneousRollPitchYaw( const Pose& p )
// --------------------------------------------------------------------------
{
  return HomogeneousRollPitchYaw(p.theta_x,p.theta_y,p.theta_z);
}


// --------------------------------------------------------------------------
Matrix HomogeneousTransformation( double x, double y, double z,
                                  double thetaX, double thetaY, double thetaZ )
// --------------------------------------------------------------------------
{
  return HomogeneousTransformation( (carmen_6d_point_t) { x, y, z, thetaX, thetaY, thetaZ } );
}


// --------------------------------------------------------------------------
Matrix HomogeneousTransformation( const carmen_6d_point_t& p )
// --------------------------------------------------------------------------
{
  Matrix M(4,4);
  M(4,1) = M(4,2) = M(4,3) = 0.0;
  M(4,4) =                   1.0;

  // //////////////////////////////////////////////////////
  // Fill translation column
  M(1,4) = p.x;
  M(2,4) = p.y;
  M(3,4) = p.z;

  // //////////////////////////////////////////////////////
  // Fill rotation part of matrix (upper left 3x3 block)
  const double cos_x = cos( p.theta_x );
  const double sin_x = sin( p.theta_x );
  const double cos_y = cos( p.theta_y );
  const double sin_y = sin( p.theta_y );
  const double cos_z = cos( p.theta_z );
  const double sin_z = sin( p.theta_z );

  const double cos_z_sin_y = cos_z * sin_y;
  const double sin_z_sin_y = sin_z * sin_y;

  M(1,1) = cos_z * cos_y;
  M(1,2) = cos_z_sin_y * sin_x - sin_z * cos_x;
  M(1,3) = cos_z_sin_y * cos_x + sin_z * sin_x;

  M(2,1) = sin_z * cos_y;
  M(2,2) = sin_z_sin_y * sin_x + cos_z * cos_x;
  M(2,3) = sin_z_sin_y * cos_x - cos_z * sin_x;

  M(3,1) = -sin_y;
  M(3,2) =  cos_y * sin_x;
  M(3,3) =  cos_y * cos_x;

  return M;
}


/**
 * @see "Computing Euler angles from a rotation matrix" by Gregory G. Slabaugh
 */
void getRPYAngles( const Matrix& M, Pose& p1, Pose& p2 )
{
  static const double ERROR_EPS = 1e-16;

  p1.invalidate();
  p2.invalidate();

  if( M.Ncols() < 3 || M.Ncols() > 4 || M.Nrows() > 4 || M.Nrows() < 3 ) {
    return;
  }

  double theta_x1, theta_x2, theta_y1, theta_y2, theta_z1, theta_z2;

  // if M(3,1) != +-1
  if( fabs(fabs(M(3,1)) - 1.0) > ERROR_EPS ) {
    // compute first solution
    theta_y1 = -asin( M(3,1) );
    const double reci_cy1 = 1./cos(theta_y1);
    theta_x1 = atan2( M(3,2)*reci_cy1, M(3,3)*reci_cy1 );
    theta_z1 = atan2( M(2,1)*reci_cy1 , M(1,1)*reci_cy1 );
    // compute second solution
    theta_y2 = M_PI - theta_y1;
    const double reci_cy2 = 1./cos(theta_y2);
    theta_x2 = atan2( M(3,2)*reci_cy2, M(3,3)*reci_cy2 );
    theta_z2 = atan2( M(2,1)*reci_cy2, M(1,1)*reci_cy2 );
    // return second solution
    p2.theta_x = theta_x2;
    p2.theta_y = theta_y2;
    p2.theta_z = theta_z2;
    p2.x = 0.; p2.y = 0.; p2.z = 0;
  } else { // if M(3,1) == +-1 (gimbal lock!)
    // set theta_z1 to any number
    theta_z1 = 0;
    // if M(3,1) == -1
    if( M(3,1) < 0 ) {
      theta_y1 = 0.5 * M_PI;
      theta_x1 = theta_z1 + atan2( M(1,2), M(1,3) );
    } else { // if M(3,1) == +1
      theta_y1 = -0.5 * M_PI;
      theta_x1 = -theta_z1 + atan2( -M(1,2), -M(1,3) );
    }
  }
  // return first solution
  p1.x = 0.; p1.y = 0.; p1.z = 0;
  p1.theta_x = theta_x1;
  p1.theta_y = theta_y1;
  p1.theta_z = theta_z1;
}





