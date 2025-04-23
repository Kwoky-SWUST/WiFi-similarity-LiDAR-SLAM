#ifndef SCIROCO__UTILS__GEO_VECTOR_3
#define SCIROCO__UTILS__GEO_VECTOR_3

#include <ostream>
#include <math.h>
#include <assert.h>

#include <carmen/carmen.h>
#include "point_extension.h"
#include "pose.h"
#include "vector2d.h"

// Switch to turn on division as multiplication of reciprocal
#ifndef SCIROCO__UTILS__GEO_VECTOR_USE_RECIPROCAL
#define SCIROCO__UTILS__GEO_VECTOR_USE_RECIPROCAL 0
#endif

const static double ERROR_EPS = 1e-16;

template < typename T >
class Vector3;

template < typename T >
std::ostream & operator << (std::ostream &, const Vector3<T> &);

/** Template class for 3-dimensional vectors of type T
 *  @author Artur Koch
 *  @todo   comparison operators: extend to machine epsilon, e.g. http://stackoverflow.com/questions/17333/most-effective-way-for-float-and-double-comparison
 */
template< typename T >
class Vector3
{
  public:

    /// data fields as union
    union {
     struct {
        T x, y, z;
      };
      T m_data[3];
    };

    /// constructors
    Vector3();
    //Vector3( const T scalar );
    template< typename T2 >
    Vector3( const Vector3<T2>& other );
    Vector3( const T x, const T y, const T z = 0.0 );
    Vector3( const Vector2<T>& v, const T z = 0. );
    Vector3( T const * const data_ptr );
#ifdef AMBISENSE_POSE_UTILS
    Vector3( const Pose& p );
#endif
#ifdef CARMEN_H
    Vector3( const carmen_point_t& p, const T z = 0.0 );
#endif
#ifdef POINT_EXTENSION
    Vector3( const carmen_6d_point_t& p );
#endif

    /// operators (more operators defined outside class scope)
    template< typename T2 >
    Vector3<T>& operator=(const Vector3<T2>& other);

    const T& operator[]( const size_t index ) const { assert(index < 3); return m_data[index]; }
    T& operator[]( const size_t index ) { assert(index < 3); return m_data[index]; }

    template< typename T2 >
    Vector3<T>& operator+=(const Vector3<T2>& other);
    template< typename T2 >
    Vector3<T>& operator-=(const Vector3<T2>& other);
    template< typename T2 >
    Vector3<T>& operator*=(const T2 scalar);
    template< typename T2 >
    Vector3<T>& operator/=(const T2 scalar);

    template< typename T2 >
    const Vector3<T> operator+(const Vector3<T2>& other) const;
    template< typename T2 >
    const Vector3<T> operator-(const Vector3<T2>& other) const;
    template< typename T2 >
    const Vector3<T> operator/(const T2 scalar) const;
    template< typename T2 >
    const Vector3<T> operator*(const T2 scalar) const;

    friend std::ostream& operator<< <>( std::ostream& ostr, const Vector3<T>& v );

    /// handy functions
    T dot( const Vector3<T>& v ) const;
    const Vector3<T> cross( const Vector3<T>& w ) const;
    bool isSameDirectionalVectorAs( const Vector3<T>& v ) const;
    bool isCollinear( const Vector3<T>& v ) const;

    T length() const;
    T length_squared() const; // dot with self

    void normalize();
    const Vector3<T> normalized() const;

    const Vector3<T> max(const Vector3<T>& other) const; // componentwise!
    const Vector3<T> min(const Vector3<T>& other) const; // componentwise!

#ifdef CARMEN_H
    // discards z-component!
    carmen_point_t asCarmenPointType() const;
#endif
    // discards z-component!
    Vector2<T> asVector2() const;


    /// predefined vectors
    static const Vector3<T> ZEROES;
    static const Vector3<T> ONES;
    static const Vector3<T> UNIT_X;
    static const Vector3<T> UNIT_Y;
    static const Vector3<T> UNIT_Z;
};


template < typename T > const Vector3<T> Vector3<T>::ZEROES( (T)0, (T)0, (T)0 );
template < typename T > const Vector3<T> Vector3<T>::ONES  ( (T)1, (T)1, (T)1 );
template < typename T > const Vector3<T> Vector3<T>::UNIT_X( (T)1, (T)0, (T)0 );
template < typename T > const Vector3<T> Vector3<T>::UNIT_Y( (T)0, (T)1, (T)0 );
template < typename T > const Vector3<T> Vector3<T>::UNIT_Z( (T)0, (T)0, (T)1 );


/****************************************************
 * Constructors
 ****************************************************/
template< typename T >
inline Vector3<T>::Vector3() : x(0), y(0), z(0) {}
//template< typename T >
//inline Vector3<T>::Vector3( const T scalar ) : x(scalar), y(scalar), z(scalar) {}
template< typename T >
template< typename T2 >
inline Vector3<T>::Vector3( const Vector3<T2>& other ) : x(other.x), y(other.y), z(other.z) {}
template< typename T >
inline Vector3<T>::Vector3( const T _x, const T _y, const T _z ) : x(_x), y(_y), z(_z) {}
template< typename T >
inline Vector3<T>::Vector3( const Vector2<T>& v, const T _z /*= 0. */ ) : x(v.x), y(v.y), z(_z) {}
template< typename T >
inline Vector3<T>::Vector3( T const * const data_ptr ) {
  if( data_ptr != NULL ) {
    m_data[0] = data_ptr[0];
    m_data[1] = data_ptr[1];
    m_data[2] = data_ptr[2];
  } else {
    x = 0; y = 0; z = 0;
  }
}
#ifdef AMBISENSE_POSE_UTILS
template< typename T >
inline Vector3<T>::Vector3( const Pose& p ) : x(p.x), y(p.y), z(p.z) {}
#endif
#ifdef CARMEN_H
template< typename T >
inline Vector3<T>::Vector3( const carmen_point_t& p, const T _z /* = 0 */ ) : x(p.x), y(p.y), z(_z) {}
#endif
#ifdef POINT_EXTENSION
template< typename T >
inline Vector3<T>::Vector3( const carmen_6d_point_t& p ) : x(p.x), y(p.y), z(p.z) {}
#endif


/****************************************************
 * Member operators
 ****************************************************/

template< typename T >
template< typename T2 >
inline Vector3<T>& Vector3<T>::operator=(const Vector3<T2>& other) {
  if(this != &other) {
    this->x = T(other.x);
    this->y = T(other.y);
    this->z = T(other.z);
  }
  return *this;
}

template< typename T >
template< typename T2 >
inline Vector3<T>& Vector3<T>::operator+=(const Vector3<T2>& other) {
  this->x += T(other.x);
  this->y += T(other.y);
  this->z += T(other.z);
  return *this;
}

template< typename T >
template< typename T2 >
inline Vector3<T>& Vector3<T>::operator-=(const Vector3<T2>& other) {
  this->y -= T(other.y);
  this->x -= T(other.x);
  this->z -= T(other.z);
  return *this;
}

template< typename T >
template< typename T2 >
inline Vector3<T>& Vector3<T>::operator*=(const T2 scalar) {
  x *= scalar;
  y *= scalar;
  z *= scalar;
  return (*this);
}

template< typename T >
template< typename T2 >
inline Vector3<T>& Vector3<T>::operator/=(const T2 scalar) {
#if SCIROCO__UTILS__GEO_VECTOR_USE_RECIPROCAL
  const T recip_t = 1./scalar;
  x *= recip_t;
  y *= recip_t;
  z *= recip_t;
#else
  x /= scalar;
  y /= scalar;
  z /= scalar;
#endif
  return (*this);
}

template< typename T >
template< typename T2 >
inline const Vector3<T> Vector3<T>::operator+(const Vector3<T2>& other) const {
  return Vector3<T>(this->x+other.x,this->y+other.y, this->z+other.z);
}

template< typename T >
template< typename T2 >
inline const Vector3<T> Vector3<T>::operator-(const Vector3<T2>& other) const {
  return Vector3<T>(this->x-other.x,this->y-other.y, this->z-other.z);
}

template< typename T >
template< typename T2 >
inline const Vector3<T> Vector3<T>::operator/(const T2 scalar) const {
#if SCIROCO__UTILS__GEO_VECTOR_USE_RECIPROCAL
  const T recip_t = 1./scalar;
  return (Vector3<T>(x * recip_t, y * recip_t, z * recip_t));
#else
  return (Vector3<T>(x/scalar, y/scalar, z/scalar));
#endif
}

template< typename T >
template< typename T2 >
inline const Vector3<T> Vector3<T>::operator*(const T2 scalar) const {
  return (Vector3<T>(x*scalar, y*scalar, z*scalar));
}


/****************************************************
 * Useful member methods
 ****************************************************/

template< typename T >
inline T Vector3<T>::dot( const Vector3<T>& v ) const {
  return v.x * x + v.y * y + v.z * z;
}

template< typename T >
inline const Vector3<T> Vector3<T>::cross( const Vector3<T>& v ) const {
  return (Vector3<T>( y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x ));
}

template< typename T >
inline bool Vector3<T>::isSameDirectionalVectorAs( const Vector3<T>& v ) const {
  return ( fabs(this->normalized().dot( v.normalized() ) - 1) <= ERROR_EPS );
}

template< typename T >
inline bool Vector3<T>::isCollinear( const Vector3<T>& v ) const {
  return ( this->isSameDirectionalVectorAs( v ) || this->isSameDirectionalVectorAs( -v ) );
}

template< typename T >
inline T Vector3<T>::length() const {
  return sqrt(x * x + y * y + z * z);
}

template< typename T >
inline T Vector3<T>::length_squared() const {
  return x * x + y * y + z * z;
}

template< typename T >
inline void Vector3<T>::normalize() {
#if SCIROCO__UTILS__GEO_VECTOR_USE_RECIPROCAL
  (*this) *= (1.0 / this->length());
#else
  (*this) /= this->length();
#endif
}

template< typename T >
inline const Vector3<T> Vector3<T>::normalized() const {
#if SCIROCO__UTILS__GEO_VECTOR_USE_RECIPROCAL
  return (*this) * (1.0 / length());
#else
  return (*this) / length();
#endif
}

template< typename T >
inline const Vector3<T> Vector3<T>::max(const Vector3<T>& other) const {
  return Vector3<T>( (x > other.x) ? x : other.x, (y > other.y) ? y : other.y, (z > other.z) ? z : other.z );
}

template< typename T >
inline const Vector3<T> Vector3<T>::min(const Vector3<T>& other) const {
  return Vector3<T>( (x < other.x) ? x : other.x, (y < other.y) ? y : other.y, (z < other.z) ? z : other.z );
}


/****************************************************
 * Type conversion methods
 ****************************************************/

#ifdef CARMEN_H
template< typename T >
inline carmen_point_t Vector3<T>::asCarmenPointType() const {
  carmen_point_t p;
  p.x = x;
  p.y = y;
  p.theta = 0;
  return p;
}
#endif

template< typename T >
Vector2<T> Vector3<T>::asVector2() const {
  return Vector2<T>( x, y );
}

/****************************************************
 * Useful non-member methods
 ****************************************************/

template< typename T >
inline const Vector3<T> floor( const Vector3<T>& v ) {
  return Vector3<T>( floor(v.x), floor(v.y), floor(v.z) );
}

template< typename T >
inline const Vector3<T> ceil( const Vector3<T>& v ) {
  return Vector3<T>( ceil(v.x), ceil(v.y), ceil(v.z) );
}

template< typename T >
inline const Vector3<T> round( const Vector3<T>& v ) {
  return Vector3<T>( floor( v + Vector3<T>(0.5) ) );
}

template< typename T >
inline const Vector3<T> abs( const Vector3<T>& v ) {
  return Vector3<T>( fabs(v.x), fabs(v.y), fabs(v.z) );
}


/****************************************************
 * Non-member operators
 ****************************************************/

/// arithmetic

template< typename T >
inline const Vector3<T> operator-(const Vector3<T> &vector) {
  return Vector3<T>( -vector.x, -vector.y, -vector.z );
}

template< typename T, typename U >
inline const Vector3<T> operator*(const U scalar, const Vector3<T>& v) {
  return Vector3<T>(scalar * v.x, scalar * v.y, scalar * v.z);
}

/// comparison

template< typename T >
inline bool operator ==( const Vector3<T>& lhs, const Vector3<T>& rhs ) {
  return ((lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z));
}

// ordering operator for stl containers: ordered by z, then by y, then by x!
template< typename T >
inline bool operator<( const Vector3<T>& lhs, const Vector3<T>& rhs ) {
  if( lhs.z > rhs.z )
    return false;
  if( lhs.z == rhs.z && lhs.y > rhs.y )
    return false;
  if( lhs.z == rhs.z && lhs.y == rhs.y && lhs.x >= rhs.x )
    return false;
  return true;
}

template< typename T >
inline bool operator!=(const Vector3<T>& lhs, const Vector3<T>& rhs) {
  return !operator==(lhs,rhs);
}

template< typename T >
inline bool operator> (const Vector3<T>& lhs, const Vector3<T>& rhs) {
  return  operator< (rhs,lhs);
}

template< typename T >
inline bool operator<=(const Vector3<T>& lhs, const Vector3<T>& rhs) {
  return !operator> (lhs,rhs);
}

template< typename T >
inline bool operator>=(const Vector3<T>& lhs, const Vector3<T>& rhs) {
  return !operator< (lhs,rhs);
}


/// ostream
template< typename T >
inline std::ostream& operator<<( std::ostream& ostr, const Vector3<T>& v ) {
  ostr << "(" << v.x << ", " << v.y << ", " << v.z << ")";
  return ostr;
}


/****************************************************
 * Typedefs
 ****************************************************/

typedef Vector3< double >         Vector3d;
typedef Vector3< float >          Vector3f;
typedef Vector3< int >            Vector3i;
typedef Vector3< unsigned int >   Vector3u;

#endif // SCIROCO__UTILS__GEO_VECTOR_3
