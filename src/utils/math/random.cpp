#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>


double random01()
{
  return ((double) rand() / RAND_MAX);
}


double random11()
{
  return ((double) rand() / RAND_MAX) * 2.0 - 1.0;
}


void sciroco_generate_time_based_random_seed()
{
  unsigned int seed;

  struct timeval tv;

  if ( gettimeofday(&tv, NULL) < 0 )
    fprintf( stderr, "error in gettimeofday : %s\n", strerror( errno) );
  seed = tv.tv_sec + tv.tv_usec;
  srand( seed );
}
