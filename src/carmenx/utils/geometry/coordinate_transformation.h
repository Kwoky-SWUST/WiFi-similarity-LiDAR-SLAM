#ifndef SCIROCO__COORDINATE_TRANSFORMATION_H
#define SCIROCO__COORDINATE_TRANSFORMATION_H

#include <utils/geometry/pose.h>
#include <newmat/newmat.h>
#include <newmat/newmatap.h>


/** Return a homogeneous 4x4 matrix that represents a translation by
 *  the vector (x,y,z) and a rotation
 *  first by thetaX around the x-axis, then by thetaY around the y-axis,
 *  and finally by thetaZ around the z-axis.
 */
Matrix HomogeneousTransformation( double x, double y, double z,
                                  double thetaX, double thetaY, double thetaZ );


/** Return a homogeneous 4x4 matrix that represents a translation by
 *  the vector (p.x, p.y, p.z) and a rotation
 *  first by p.theta_x around the x-axis, then by p.theta_y around the y-axis,
 *  and finally by p.theta_z around the z-axis.
 */
Matrix HomogeneousTransformation( const carmen_6d_point_t& p );


/** Return a homogeneous 4x4 matrix that represents a translation by
 *  the vector (x,y,z).
 */
Matrix HomogeneousTranslation( double x, double y, double z );


/** Return a homogeneous 4x4 matrix that represents a rotation
 *  first by thetaX around the x-axis, then by thetaY around the y-axis,
 *  and finally by thetaZ around the z-axis
 */
Matrix HomogeneousRollPitchYaw( double thetaX, double thetaY, double thetaZ );
Matrix HomogeneousRollPitchYaw( const carmen_6d_point_t& p );
Matrix HomogeneousRollPitchYaw( const Pose& p );


/** Compute RPY angles from a homogeneous 4x4 matrix
 * @param p1 returns first solution
 * @param p2 returns second solution if available, else p2 is set to invalid
 * @see "Computing Euler angles from a rotation matrix" by Gregory G. Slabaugh
 */
void getRPYAngles( const Matrix& M, Pose& p1, Pose& p2 );



#endif // SCIROCO__COORDINATE_TRANSFORMATION_H
