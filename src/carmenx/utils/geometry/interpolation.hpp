#if _MSC_VER > 1000
#pragma once
#endif

#ifndef HPP_INTERPOL_43285893
#define HPP_INTERPOL_43285893

#include <vector>

#include <utils/geometry/vector2d.h>
#include <utils/geometry/vector3d.h>

template<class T> class Interpolator
{
  public:
    /** Linear interpolation
     * @param target the target point, 0.0 - 1.0
     * @param v a pointer to an array of size 2 containing the two values
     * @return linear interpolated target value
     */
    inline static T Linear( double target, T const * const v ) {
      return (T)(target*(v[1]) + (1.0 - target)*(v[0]));
    }
    inline static T Linear( double target, const std::vector< T >& v ) {
      return (T)(target*(v[1]) + (1.0 - target)*(v[0]));
    }

    /** BiLinear interpolation, linear interpolation in 2D
     * @param target a 2D point (X,Y)
     * @param v an array of size 4 containing values clockwise around the square starting from bottom left
     * @return bilinear interpolated target value
     */
    inline static T Bilinear( double *target, T const * const v ) {
      const T v_prime[2] = { Linear(target[1], &(v[0])),
                             Linear(1.0 - target[1], &(v[2])) };
      return Linear(target[0], v_prime);
    }
    inline static T Bilinear( const std::vector< double >& target, const std::vector< T >& v ) {
      const T v_prime[2] = { Linear(target[1], &(v.data()[0])),
                             Linear(1.0 - target[1], &(v.data()[2])) };
      return Linear(target[0], v_prime);
    }

    /** TriLinear interpolation, linear interpolation in 3D
     * @param target a 3D point (X,Y,Z)
     * @param v an array of size 8 containing the values of the 8 corners of a cube
     *          defined as two clockwise faces in the XY-plane: 0-3 face one (front face), 4-7 face two (back face)
     *          e.g. using right-handed coordinate frame (XYZ), face1 should be: { 000, 010, 110, 100 } and face2: { 001, 011, 111, 101 }
     * @return trilinear interpolated target value
     */
    inline static T Trilinear( double *target, T const * const v ) {
      const T v_prime[2] = { Bilinear(&(target[0]), &(v[0])),
                             Bilinear(&(target[0]), &(v[4])) };
      return Linear(target[2], v_prime);
    }
    inline static T Trilinear( const std::vector<double>& target, const std::vector< T >& v ) {
      const T v_prime[2] = { Bilinear(&(target.data()[0]), &(v.data()[0])),
                             Bilinear(&(target.data()[0]), &(v.data()[4])) };
      return Linear(target[2], v_prime);
    }
};



/****************************************************
 * Interpolation algorithms for 1d/2d/3d std::vectors of double
 ****************************************************/

namespace std_ext {
  double getLinearInterpolated( const double target, const std::vector< double >& vec1d ) {
//    assert( target >= 0 );
//    assert( target <= vec.size() );
    const int low = static_cast< int >( floor( target ) );
    const int high = low + 1;
    if( low < 0 ) return vec1d[high];
    else if( static_cast<unsigned>(high) >= vec1d.size() ) return vec1d[low];
    const double lowVal  = vec1d[low];
    const double highVal = vec1d[high];
    const double weight = target - floor( target );
    return (weight*(highVal) + (1.0 - weight)*(lowVal));
  }

  double getBilinearInterpolated( const double target_dim1, const double target_dim2, const std::vector< std::vector< double > >& vec2d ) {
//    assert( target_dim1 >= 0 );
//    assert( target_dim1 <= vec2d.size() );
    const int low = static_cast< int >( floor( target_dim1 ) );
    const int high = low + 1;
    if( low < 0 ) return getLinearInterpolated( target_dim2, vec2d[high] );
    else if( static_cast<unsigned>(high) >= vec2d.size() ) return getLinearInterpolated( target_dim2, vec2d[low] );
    const double lowVal  = getLinearInterpolated( target_dim2, vec2d[low] );
    const double highVal = getLinearInterpolated( target_dim2, vec2d[high] );
    const double weight = target_dim1 - floor( target_dim1 );
    return (weight*(highVal) + (1.0 - weight)*(lowVal));
  }

  double getBilinearInterpolated( const Vector2d& target2d, const std::vector< std::vector< double > >& vec2d ) {
    return getBilinearInterpolated( target2d.x, target2d.y, vec2d );
  }

  double getTrilinearInterpolated( const double target_dim1,
                                   const double target_dim2,
                                   const double target_dim3,
                                   const std::vector< std::vector< std::vector< double > > >& vec3d ) {
//    assert( target_dim1 >= 0 );
//    assert( target_dim1 <= vec3d.size() );
    const int low = static_cast< int >( floor( target_dim1 ) );
    const int high = low + 1;
    if( low < 0 ) return getBilinearInterpolated( target_dim2, target_dim3, vec3d[ high ] );
    else if( static_cast<unsigned>(high) >= vec3d.size() ) return getBilinearInterpolated( target_dim2, target_dim3, vec3d[ low ] );
    const double lowVal  = getBilinearInterpolated( target_dim2, target_dim3, vec3d[ low ] );
    const double highVal = getBilinearInterpolated( target_dim2, target_dim3, vec3d[ high ] );
    const double weight = target_dim1 - floor( target_dim1 );
    return (weight*(highVal) + (1.0 - weight)*(lowVal));
  }

  double getTrilinearInterpolated( const Vector3d& target3d,
                                   const std::vector< std::vector< std::vector< double > > >& vec3d ) {
    return getTrilinearInterpolated( target3d.x, target3d.y, target3d.z, vec3d );
  }

}

#endif // guard
