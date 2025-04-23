#include <time.h>
#include <errno.h>

#include <iostream>

using namespace std;


#define BILLION 1000000000

void nanowait( int sec, long int nanosec )
{
  // Ensure validity of parameters
  if ( sec < 0 ) sec = 0;
  sec    += nanosec / BILLION;
  nanosec = nanosec % BILLION;

  // Information for nanosleep
  struct timespec wait_time;
  wait_time.tv_sec  = sec;
  wait_time.tv_nsec = nanosec;

  int success;

  bool do_continue = false;
  do
  {
    // Try to sleep for the specified amount of time and retrieve remaining
    // time in the same struct
    success = nanosleep( &wait_time, &wait_time );

    // Do we have to recall nanosleep because it was interrupted by a signal?
    if ( success == -1 && errno == EINTR )
      do_continue = true;
    else
      do_continue = false;

  } while ( do_continue );
}
