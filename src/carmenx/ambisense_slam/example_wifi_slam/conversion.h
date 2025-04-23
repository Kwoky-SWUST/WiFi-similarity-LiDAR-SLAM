#ifndef AMBISENSE_MATRIX_CONVERSION_H
#define AMBISENSE_MATRIX_CONVERSION_H

/**
 * Converts a Euler angles to 3x3 matrix.
 * @param     input euler angle,Euler_x,Euler_y,Euler_z format
 * @param mat  output 3*3 matrix
 * @see SLAM6D include/globals.icc
 */
// --------------------------------------------------------------------------
void EulerZYXTOMatrix(const double eulerZ, const double eulerY, const double eulerX,
		double *mat)
// --------------------------------------------------------------------------
{
	double ci = cos(eulerX);
	double cj = cos(eulerY);
	double ch = cos(eulerZ);
	double si = sin(eulerX);
	double sj = sin(eulerY);
	double sh = sin(eulerZ);
	double cc = ci * ch;
	double cs = ci * sh;
	double sc = si * ch;
	double ss = si * sh;

	mat[0] = cj * ch;
	mat[1] = cj * sh;
	mat[2] = -sj;

	mat[3] = sj * sc - cs;
	mat[4] = sj * ss + cc;
	mat[5] = cj * si;

	mat[6] = sj * cc + ss;
	mat[7] = sj * cs - sc;
	mat[8] = cj * ci;

}


/**
 * Converts yaw, pitch, and roll to 3x3 matrix.
 * @param input yaw pitch and roll
 * @param mat  output 3*3 matrix
 * @see SLAM6D include/globals.icc
 */
// --------------------------------------------------------------------------
void EulerYPRTOMatrix(const double yaw, const double pitch, const double roll,
		double *mat)
// --------------------------------------------------------------------------
{
	double eulerZ=yaw;
	double eulerY=pitch;

	double eulerX=roll;
	double ci = cos(eulerX);
	double cj = cos(eulerY);
	double ch = cos(eulerZ);
	double si = sin(eulerX);
	double sj = sin(eulerY);
	double sh = sin(eulerZ);
	double cc = ci * ch;
	double cs = ci * sh;
	double sc = si * ch;
	double ss = si * sh;

	mat[0] = cj * ch;
	mat[1] = cj * sh;
	mat[2] = -sj;

	mat[3] = sj * sc - cs;
	mat[4] = sj * ss + cc;
	mat[5] = cj * si;

	mat[6] = sj * cc + ss;
	mat[7] = sj * cs - sc;
	mat[8] = cj * ci;

}

/**
 * Converts a quaternion to a 3x3 matrix.
 * @param quat    input quaternion,x,y,z,w format
 * @param mat  output 3*3 matrix
 * @see SLAM6D include/globals.icc
 */
void QuatToMatrix3(const double *quat,double *mat)
{
	double x=quat[0];
	double y=quat[1];
	double z=quat[2];
	double w=quat[3];
	double d2 = x*x+y*y+z*z+w*w;
	double s = 2.0 / d2;
	double xs = x * s,   ys = y * s,   zs = z * s;
	double wx = w * xs,  wy = w * ys,  wz = w * zs;
	double xx = x * xs,  xy = x * ys,  xz = x * zs;
	double yy = y * ys,  yz = y * zs,  zz = z * zs;

	mat[0] = 1.0 - (yy + zz);
	mat[1] = xy + wz;
	mat[2] = xz - wy;

	mat[3] = xy - wz;
	mat[4] = 1.0 - (xx + zz);
	mat[5] = yz + wx;

	mat[6] = xz + wy;
	mat[7] = yz - wx;
	mat[8] = 1.0 - (xx + yy);
}


/**
 * Transforms a 3x3 Transformation Matrix into a quaternion
 *
 * @param mat     matrix to be converted
 * @param quat    resulting quaternion
 */
void Matrix3ToQuat(const double *mat,
                                 double *quat)
{

  double T, S, X, Y, Z, W;
  T = 1 + mat[0] + mat[4] + mat[8];
  if ( T > 0.00000001 ) { // to avoid large distortions!
    S = sqrt(T) * 2;
    X = ( mat[7] - mat[5] ) / S;
    Y = ( mat[2] - mat[6] ) / S;
    Z = ( mat[3] - mat[1] ) / S;
    W = 0.25 * S;
  } else if ( mat[0] > mat[4] && mat[0] > mat[8] )  { // Column 0:
    S  = sqrt( 1.0 + mat[0] - mat[4] - mat[8] ) * 2;
    X = 0.25 * S;
    Y = (mat[3] + mat[1] ) / S;
    Z = (mat[2] + mat[6] ) / S;
    W = (mat[7] - mat[5] ) / S;
  } else if ( mat[4] > mat[8] ) {                    // Column 1:
    S  = sqrt( 1.0 + mat[4] - mat[0] - mat[8] ) * 2;
    X = (mat[3] + mat[1] ) / S;
    Y = 0.25 * S;
    Z = (mat[7] + mat[5] ) / S;
    W = (mat[2] - mat[6] ) / S;
  } else {                                            // Column 2:
    S  = sqrt( 1.0 + mat[8] - mat[0] - mat[4] ) * 2;
    X = (mat[2] + mat[6] ) / S;
    Y = (mat[7] + mat[5] ) / S;
    Z = 0.25 * S;
    W = (mat[3] - mat[1] ) / S;
  }
  quat[0] = W;
  quat[1] = -X;
  quat[2] = -Y;
  quat[3] = -Z;
  double norm = sqrt((quat[0]*quat[0] + quat[1]*quat[1] + quat[2]*quat[2] + quat[3]*quat[3]));

  quat[0] = quat[0]/norm;
  quat[1] = quat[1]/norm;
  quat[2] = quat[2]/norm;
  quat[3] = quat[3]/norm;
}



/**
 * Converts a 3x3 matrix to Euler angles.
 *
 * @param mat    input 3x3 matrix
 * @param theta  output 3-vector of Euler angles, euler_x, euler_y, euler_z
 * @see SLAM6D include/globals.icc
 */
void Matrix4ToEuler(const double *mat,
		double *theta)
{

  double _trX, _trY;

  // Calculate Y-axis angle
  if(mat[0] > 0.0) {
    theta[1] = asin(mat[6]);
  } else {
    theta[1] = M_PI - asin(mat[6]);
  }

  double  C    =  cos( theta[1] );
  if ( fabs( C ) > 0.005 )  {                 // Gimball lock?
    _trX      =  mat[8] / C;             // No, so get X-axis angle
    _trY      =  -mat[7] / C;
    theta[0]  = atan2( _trY, _trX );
    _trX      =  mat[0] / C;              // Get Z-axis angle
    _trY      = -mat[3] / C;
    theta[2]  = atan2( _trY, _trX );
  } else {                                    // Gimball lock has occurred
    theta[0] = 0.0;                       // Set X-axis angle to zero
    _trX      =  mat[4];  //1                // And calculate Z-axis angle
    _trY      =  mat[1];  //2
    theta[2]  = atan2( _trY, _trX );
  }

  theta[0] = theta[0];//euler x
  theta[1] = theta[1];//euler y
  theta[2] = theta[2];//euler z
}



/**
 * Converts a 3x3 matrix to Euler angles.
 *
 * @param mat    input 3x3 matrix
 * @param theta  output 3-vector of Euler angles, euler_x, euler_y, euler_z
 * @see SLAM6D include/globals.icc
 */
void Matrix3ToEuler(const double *mat,
		double *theta)
{

  double _trX, _trY;

  // Calculate Y-axis angle
  if(mat[0] > 0.0) {
    theta[1] = asin(mat[6]);
  } else {
    theta[1] = M_PI - asin(mat[6]);
  }

  double  C    =  cos( theta[1] );
  if ( fabs( C ) > 0.005 )  {                 // Gimball lock?
    _trX      =  mat[8] / C;             // No, so get X-axis angle
    _trY      =  -mat[7] / C;
    theta[0]  = atan2( _trY, _trX );
    _trX      =  mat[0] / C;              // Get Z-axis angle
    _trY      = -mat[3] / C;
    theta[2]  = atan2( _trY, _trX );
  } else {                                    // Gimball lock has occurred
    theta[0] = 0.0;                       // Set X-axis angle to zero
    _trX      =  mat[4];  //1                // And calculate Z-axis angle
    _trY      =  mat[1];  //2
    theta[2]  = atan2( _trY, _trX );
  }

  theta[0] = theta[0];//euler x
  theta[1] = theta[1];//euler y
  theta[2] = theta[2];//euler z
}



/**
 * Converts yaw, pitch, and roll angles to 3x3 matrix.
 *
 * @param input yaw (euler_z, rotation along z), pitch (euler_y, rotation along y), roll (euler_x, rotation along x),
 * @param output quaternion, x, y, z, w 
 * @see SLAM6D include/globals.icc
 */
// --------------------------------------------------------------------------
void EulerYPRTOQuat(const double yaw, const double pitch, const double roll,
		double *quat)
// --------------------------------------------------------------------------
{
	double halfYaw = yaw * 0.5;
	double halfPitch = pitch * 0.5;
	double halfRoll = roll * 0.5;
	double cosYaw = cos(halfYaw);
	double sinYaw = sin(halfYaw);
	double cosPitch = cos(halfPitch);
	double sinPitch = sin(halfPitch);
	double cosRoll = cos(halfRoll);
	double sinRoll = sin(halfRoll);

	quat[0] = cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw;//x
	quat[1] = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw;//y
	quat[2] = sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw;//z
	quat[3] = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw;//w

}




#endif
