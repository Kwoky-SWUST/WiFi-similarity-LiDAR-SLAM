#include <utils/math/fast_sqrt.h>
#include <utils/debug/assert.h>
#include <cmath>

#define FABS(f) ( (f>0.0f) ? f : -f )

#include <iostream>

// --------------------------------------------------------------------------
float fast_sqrt( float f )
// --------------------------------------------------------------------------
{
  ASSERT( ! isinf(f), "fast_sqrt(f) for f=" << f );
  ASSERT( ! isnan(f), "fast_sqrt(f) for f=" << f );
  ASSERT( f >= 0.0f,  "fast_sqrt(f) for f=" << f );

  if ( f == 0.0 )
    return 0.0;

  float z = 1.0f;
  float y;
  float reciprocal_z;
  float relative_error;
  do
  {
    y  = z*z - f;
    reciprocal_z  = 1.0f / z;
    z -= y * 0.5f * reciprocal_z;
    relative_error = y * reciprocal_z;
/*        std::cerr << " [ sqrt(" << f << " ~ " << z << " @" << y << "]";*/
  } while ( FABS( relative_error ) > 0.01f );

  return z;
}
