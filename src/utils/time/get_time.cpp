#include <utils/time/get_time.h>
#include <stdlib.h>


timeval currentGetTime;

// --------------------------------------------------------------------------
timeval & getCurrentTime()
// --------------------------------------------------------------------------
{
  gettimeofday( &currentGetTime, NULL );
  return currentGetTime;
}


// --------------------------------------------------------------------------
long unsigned int 
getCurrentSeconds()
// --------------------------------------------------------------------------
{
  return getCurrentTime().tv_sec;
}


// --------------------------------------------------------------------------
unsigned int 
getCurrentMilliSeconds()
// --------------------------------------------------------------------------
{
  return ( getCurrentTime().tv_usec / 1000 );
}


// --------------------------------------------------------------------------
unsigned int
getCurrentMicroSeconds()
// --------------------------------------------------------------------------
{
  return ( getCurrentTime().tv_usec );
}


// --------------------------------------------------------------------------
std::string
getDateAndTimeStamp()
// --------------------------------------------------------------------------
{
  char timestamp[100];
  time_t mytime;
  struct tm *mytm;
  mytime = time( NULL );
  mytm = localtime( &mytime );
  strftime( timestamp, sizeof( timestamp ),"%a, %d %b %Y %H:%M:%S %z", mytm );
  return std::string( timestamp );
}
