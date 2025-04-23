#include <carmen/carmen.h>
#include <carmen/grid_raytracer.h>
#include <iostream>

using namespace std;

#define NUM_COS_VALUES 720
double __cos_factor;
double __cos_precomputed[ NUM_COS_VALUES ];
inline double COS( const double angle )
{
  int index = lrint( angle * __cos_factor );
  while ( index < 0 )
    index += NUM_COS_VALUES;
  while ( index >= NUM_COS_VALUES )
    index -= NUM_COS_VALUES;
  return __cos_precomputed[ index ];
}


void precompute_cosine()
{
  __cos_factor = NUM_COS_VALUES * 1.0 / (2*M_PI);
  for ( int index = 0; index < NUM_COS_VALUES; ++index )
  {
    __cos_precomputed[index] = index * 2 * M_PI / NUM_COS_VALUES;
  }
}


// --------------------------------------------------------------------------
void
test_distances( GridRaytracer& rt, double x, double y )
// --------------------------------------------------------------------------
{
  const int MAX_ANGLES = 8;
  double    sqr_dist;
  carmen_point_t p = { x, y, 0.0 };
  for ( int i = 0; i < MAX_ANGLES; ++i )
  {
    p.theta = i * M_PI * 2 / MAX_ANGLES;
    sqr_dist = rt.getExpectedSquaredDistance( &p );
    cout << "Distance from " << p.x << "," << p.y << " @ " << p.theta << ": " << sqrt( sqr_dist ) << endl;
  }
}


// --------------------------------------------------------------------------
int
main( int argc, char** argv )
// --------------------------------------------------------------------------
{
  carmen_ipc_initialize( argc, argv );

  carmen_map_t map;
  carmen_map_get_gridmap( &map );

  GridRaytracer rt( &map, 0.2 );
  test_distances( rt, 8.0, 3.0 );
  test_distances( rt, 3.2, 2.5 );

  const int MAX_NUM_TESTS = 180*2000;
  double start_time = carmen_get_time();
  carmen_point_t p;
  for ( int i = 0; i < MAX_NUM_TESTS; ++i )
  {
    p.x = carmen_uniform_random( 0, 12 );
    p.y = carmen_uniform_random( 0, 6 );
    p.theta = carmen_uniform_random( 0, 2*M_PI );
    rt.getExpectedSquaredDistance( &p );
  }
  double duration = carmen_get_time() - start_time;
  cout << "Duration of " << MAX_NUM_TESTS << " raytracer calls: " << duration << " seconds" << endl;

  const int MAX_NUM_INT_BENCHMARKS = 1000000;
  int j;
  double d;
  start_time = carmen_get_time();
  for ( int i = 0; i < MAX_NUM_INT_BENCHMARKS; ++i )
  {
    d = carmen_uniform_random( 0, 1 );
    j = lrint( d );
  }
  duration = carmen_get_time() - start_time;
  cout << "Duration of " << MAX_NUM_INT_BENCHMARKS << " LRINT benchmarks: " << duration << " seconds" << endl;

  start_time = carmen_get_time();
  for ( int i = 0; i < MAX_NUM_INT_BENCHMARKS; ++i )
  {
    d = carmen_uniform_random( 0, 1 );
    j = (int) d;
  }
  duration = carmen_get_time() - start_time;
  cout << "Duration of " << MAX_NUM_INT_BENCHMARKS << " (INT) benchmarks: " << duration << " seconds" << endl;


  precompute_cosine();
  const int MAX_NUM_COS_BENCHMARKS = 1000000;
  start_time = carmen_get_time();
  for ( int i = 0; i < MAX_NUM_COS_BENCHMARKS; ++i )
  {
    d = carmen_uniform_random( 0, 2*M_PI );
    d = cos(d);
  }
  duration = carmen_get_time() - start_time;
  cout << "Duration of " << MAX_NUM_COS_BENCHMARKS << " 'cos' benchmarks: " << duration << " seconds" << endl;

  start_time = carmen_get_time();
  for ( int i = 0; i < MAX_NUM_COS_BENCHMARKS; ++i )
  {
    d = carmen_uniform_random( 0, 2*M_PI );
    d = COS(d);
  }
  duration = carmen_get_time() - start_time;
  cout << "Duration of " << MAX_NUM_COS_BENCHMARKS << " 'COS' benchmarks: " << duration << " seconds" << endl;

  return EXIT_SUCCESS;
}
