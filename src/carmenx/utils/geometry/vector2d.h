#ifndef SCIROCO__UTILS__GEO_VECTOR_2
#define SCIROCO__UTILS__GEO_VECTOR_2

#include <ostream>
#include <math.h>
#include <assert.h>

#include <carmen/carmen.h>
#include "point_extension.h"
#include "pose.h"

// Switch to turn on division as multiplication of reciprocal
#ifndef SCIROCO__UTILS__GEO_VECTOR_USE_RECIPROCAL
#define SCIROCO__UTILS__GEO_VECTOR_USE_RECIPROCAL 0
#endif

template < typename T >
class Vector2;

template < typename T >
std::ostream & operator << (std::ostream &, const Vector2<T> &);

/** Template class for 2-dimensional vectors of type T
 *  @author Artur Koch
 *  @todo   comparison operators: extend to machine epsilon, e.g. http://stackoverflow.com/questions/17333/most-effective-way-for-float-and-double-comparison
 */
template< typename T >
class Vector2
{
  public:

    /// data fields as union
    union {
     struct {
        T x, y;
      };
      T m_data[2];
    };

    /// constructors
    Vector2();
    //Vector2( const T scalar );
    Vector2( const Vector2<T>& other );
    Vector2( const T x, const T y );
    Vector2( T const * const data_ptr );
#ifdef AMBISENSE_POSE_UTILS
    Vector2( const Pose& p );
#endif
#ifdef CARMEN_H
    Vector2( const carmen_point_t& p );
#endif
#ifdef POINT_EXTENSION
    Vector2( const carmen_6d_point_t& p );
#endif

    /// operators (more operators defined outside class scope)
    Vector2<T>& operator=(const Vector2<T>& other);

    const T& operator[]( const size_t index ) const { assert(index < 2); return m_data[index]; }
    T& operator[]( const size_t index ) { assert(index < 2); return m_data[index]; }

    Vector2<T>& operator+=(const Vector2<T>& other);
    Vector2<T>& operator-=(const Vector2<T>& other);
    Vector2<T>& operator*=(const T scalar);
    Vector2<T>& operator/=(const T scalar);

    const Vector2<T> operator+(const Vector2<T>& other) const;
    const Vector2<T> operator-(const Vector2<T>& other) const;
    const Vector2<T> operator/(const T scalar) const;
    const Vector2<T> operator*(const T scalar) const;

    friend std::ostream& operator<< <>( std::ostream& ostr, const Vector2<T>& v );

    /// handy functions
    T dot( const Vector2<T>& v ) const;

    T length() const;
    T length_squared() const; // dot with self

    void normalize();
    const Vector2<T> normalized() const;

    const Vector2<T> max(const Vector2<T>& other) const; // componentwise!
    const Vector2<T> min(const Vector2<T>& other) const; // componentwise!

    // get counterclockwise normal (perp product)
    const Vector2<T> getNormal() const;
    const Vector2<T> perp() const { return this->getNormal(); }
    /** get counterclockwise normal for the vector from this to other (perp product)
     * @param other the end point of the vector for the perp product ((*this) is considered to be the start point)
     * @return the counterclockwise normal
     */
    const Vector2<T> getNormalToOther( const Vector2<T>& endPointOfVector ) const;

#ifdef CARMEN_H
    carmen_point_t asCarmenPointType() const;
#endif

    /// predefined vectors
    static const Vector2<T> ZEROES;
    static const Vector2<T> ONES;
    static const Vector2<T> UNIT_X;
    static const Vector2<T> UNIT_Y;
};


template < typename T > const Vector2<T> Vector2<T>::ZEROES( (T)0, (T)0 );
template < typename T > const Vector2<T> Vector2<T>::ONES( (T)1, (T)1 );
template < typename T > const Vector2<T> Vector2<T>::UNIT_X( (T)1, (T)0 );
template < typename T > const Vector2<T> Vector2<T>::UNIT_Y( (T)0, (T)1 );


/****************************************************
 * Constructors
 ****************************************************/
template< typename T >
inline Vector2<T>::Vector2() : x(0), y(0) {}
//template< typename T >
//inline Vector2<T>::Vector2( const T scalar ) : x(scalar), y(scalar) {}
template< typename T >
inline Vector2<T>::Vector2( const Vector2<T>& other ) : x(other.x), y(other.y) {}
template< typename T >
inline Vector2<T>::Vector2( const T _x, const T _y ) : x(_x), y(_y) {}
template< typename T >
inline Vector2<T>::Vector2( T const * const data_ptr ) {
  if( data_ptr != NULL ) {
    m_data[0] = data_ptr[0];
    m_data[1] = data_ptr[1];
  } else {
    x = 0; y = 0;
  }
}
#ifdef AMBISENSE_POSE_UTILS
template< typename T >
inline Vector2<T>::Vector2( const Pose& p ) : x(p.x), y(p.y) {}
#endif
#ifdef CARMEN_H
template< typename T >
inline Vector2<T>::Vector2( const carmen_point_t& p ) : x(p.x), y(p.y) {}
#endif
#ifdef POINT_EXTENSION
template< typename T >
inline Vector2<T>::Vector2( const carmen_6d_point_t& p ) : x(p.x), y(p.y) {}
#endif


/****************************************************
 * Member operators
 ****************************************************/

template< typename T >
inline Vector2<T>& Vector2<T>::operator=(const Vector2<T>& other) {
  if(this != &other) {
    this->x = other.x;
    this->y = other.y;
  }
  return *this;
}

template< typename T >
inline Vector2<T>& Vector2<T>::operator+=(const Vector2<T>& other) {
  this->x += other.x;
  this->y += other.y;
  return *this;
}

template< typename T >
inline Vector2<T>& Vector2<T>::operator-=(const Vector2<T>& other) {
  this->x -= other.x;
  this->y -= other.y;
  return *this;
}

template< typename T >
inline Vector2<T>& Vector2<T>::operator*=(const T scalar) {
  x *= scalar;
  y *= scalar;
  return (*this);
}

template< typename T >
inline Vector2<T>& Vector2<T>::operator/=(const T scalar) {
#if SCIROCO__UTILS__GEO_VECTOR_USE_RECIPROCAL
  const T recip_t = 1./scalar;
  x *= recip_t;
  y *= recip_t;
#else
  x /= scalar;
  y /= scalar;
#endif
  return (*this);
}

template< typename T >
inline const Vector2<T> Vector2<T>::operator+(const Vector2<T>& other) const {
  return Vector2<T>(this->x+other.x,this->y+other.y);
}

template< typename T >
inline const Vector2<T> Vector2<T>::operator-(const Vector2<T>& other) const {
  return Vector2<T>(this->x-other.x,this->y-other.y);
}

template< typename T >
inline const Vector2<T> Vector2<T>::operator/(const T scalar) const {
#if SCIROCO__UTILS__GEO_VECTOR_USE_RECIPROCAL
  const T recip_t = 1./scalar;
  return (Vector2<T>(x * recip_t, y * recip_t));
#else
  return (Vector2<T>(x / scalar, y / scalar));
#endif
}

template< typename T >
inline const Vector2<T> Vector2<T>::operator*(const T scalar) const {
  return (Vector2<T>(x * scalar, y * scalar));
}


/****************************************************
 * Useful member methods
 ****************************************************/

template< typename T >
inline T Vector2<T>::dot( const Vector2<T>& v ) const {
  return v.x * x + v.y * y;
}

template< typename T >
inline T Vector2<T>::length() const {
  return sqrt(x * x + y * y);
}

template< typename T >
inline T Vector2<T>::length_squared() const {
  return x * x + y * y;
}

template< typename T >
inline void Vector2<T>::normalize() {
#if SCIROCO__UTILS__GEO_VECTOR_USE_RECIPROCAL
  (*this) *= (1.0 / this->length());
#else
  (*this) /= this->length();
#endif
}

template< typename T >
inline const Vector2<T> Vector2<T>::normalized() const {
#if SCIROCO__UTILS__GEO_VECTOR_USE_RECIPROCAL
  return (*this) * (1.0 / length());
#else
  return (*this) / length();
#endif
}

template< typename T >
inline const Vector2<T> Vector2<T>::max(const Vector2<T>& other) const {
  return Vector2<T>( (x > other.x) ? x : other.x, (y > other.y) ? y : other.y );
}

template< typename T >
inline const Vector2<T> Vector2<T>::min(const Vector2<T>& other) const {
  return Vector2<T>( (x < other.x) ? x : other.x, (y < other.y) ? y : other.y );
}

template< typename T >
inline const Vector2<T> Vector2<T>::getNormal() const {
  return Vector2<T>( -y, x );
}

template< typename T >
inline const Vector2<T> Vector2<T>::getNormalToOther( const Vector2<T>& v1 ) const {
//  const Vector2<T> v01 = v1 - (*this);
//  return Vector2<T>( -v01.y, v01.x );
  return (v1 - (*this)).perp();
}


/****************************************************
 * Type conversion methods
 ****************************************************/

#ifdef CARMEN_H
template< typename T >
inline carmen_point_t Vector2<T>::asCarmenPointType() const {
  carmen_point_t p;
  p.x = x;
  p.y = y;
  p.theta = 0;
  return p;
}
#endif

/****************************************************
 * Useful non-member methods
 ****************************************************/

template< typename T >
inline const Vector2<T> floor( const Vector2<T>& v ) {
  return Vector2<T>( floor(v.x), floor(v.y) );
}

template< typename T >
inline const Vector2<T> ceil( const Vector2<T>& v ) {
  return Vector2<T>( ceil(v.x), ceil(v.y) );
}

template< typename T >
inline const Vector2<T> round( const Vector2<T>& v ) {
  return Vector2<T>( floor( Vector2<T>(v.x + 0.5, v.y + 0.5) ) );
}

template< typename T >
inline const Vector2<T> abs( const Vector2<T>& v ) {
  return Vector2<T>( fabs(v.x), fabs(v.y) );
}


/****************************************************
 * Non-member operators
 ****************************************************/

/// arithmetic

template< typename T >
inline const Vector2<T> operator-(const Vector2<T> &vec) {
  return Vector2<T>( -vec.x, -vec.y );
}

template< typename T >
inline const Vector2<T> operator*(const T scalar, const Vector2<T>& v) {
  return Vector2<T>(scalar * v.x, scalar * v.y);
}

/// comparison

template< typename T >
inline bool operator ==( const Vector2<T>& lhs, const Vector2<T>& rhs ) {
  return ((lhs.x == rhs.x) && (lhs.y == rhs.y));
}

// ordering operator for stl containers: ordered by y, then by x!
template< typename T >
inline bool operator<( const Vector2<T>& lhs, const Vector2<T>& rhs ) {
  if( lhs.y > rhs.y )
    return false;
  if( lhs.y == rhs.y && lhs.x >= rhs.x )
    return false;
  return true;
}

template< typename T >
inline bool operator!=(const Vector2<T>& lhs, const Vector2<T>& rhs) {
  return !operator==(lhs,rhs);
}

template< typename T >
inline bool operator> (const Vector2<T>& lhs, const Vector2<T>& rhs) {
  return  operator< (rhs,lhs);
}

template< typename T >
inline bool operator<=(const Vector2<T>& lhs, const Vector2<T>& rhs) {
  return !operator> (lhs,rhs);
}

template< typename T >
inline bool operator>=(const Vector2<T>& lhs, const Vector2<T>& rhs) {
  return !operator< (lhs,rhs);
}


/// ostream
template< typename T >
inline std::ostream& operator<<( std::ostream& ostr, const Vector2<T>& v ) {
  ostr << "(" << v.x << ", " << v.y << ")";
  return ostr;
}


/****************************************************
 * Typedefs
 ****************************************************/

typedef Vector2< double >         Vector2d;
typedef Vector2< float >          Vector2f;
typedef Vector2< int >            Vector2i;
typedef Vector2< unsigned int >   Vector2u;

#endif
