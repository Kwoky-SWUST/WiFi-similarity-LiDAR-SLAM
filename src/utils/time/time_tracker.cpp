#include <stdlib.h>
#include <utils/time/time_tracker.h>
#include <utils/text/colors.h>
#include <utils/math/math.h>
//
#include <assert.h>
#include <iomanip>
#include <iostream>
#include <cmath>

using namespace std;


// Singleton instance
static TimeTracker * timeTrackerInstance = NULL;


// --------------------------------------------------------------------------
TimeTracker * TimeTracker::getInstance()
// --------------------------------------------------------------------------
{
  if ( timeTrackerInstance == NULL )
  {
    timeTrackerInstance = new TimeTracker();
  }

  return timeTrackerInstance;
}


// --------------------------------------------------------------------------
TimeTracker::TimeTracker()
// --------------------------------------------------------------------------
{
  m_iMaxClassNameLength = 0;
  reset();
//   addClass( "Default" );
}


// --------------------------------------------------------------------------
TimeTracker::~TimeTracker()
// --------------------------------------------------------------------------
{
  reset();
}


// --------------------------------------------------------------------------
void TimeTracker::reset()
// --------------------------------------------------------------------------
{
  m_vMeasurements.clear();
}


// --------------------------------------------------------------------------
unsigned int TimeTracker::addClass( const string & name )
// --------------------------------------------------------------------------
{
  vector< double > times;
  times.clear();
  m_vMeasurements.push_back( (MeasurementClass) { name, 0.0, 0.0, INFINITY, -INFINITY, 0, current_time, current_time, times } );
  m_iMaxClassNameLength = max( m_iMaxClassNameLength, name.length() );

  return m_vMeasurements.size() - 1;
}


// --------------------------------------------------------------------------
void TimeTracker::pingStart( unsigned int cls )
// --------------------------------------------------------------------------
{
  start( cls );
}


// --------------------------------------------------------------------------
void TimeTracker::start( unsigned int cls )
// --------------------------------------------------------------------------
{
  if (cls >= m_vMeasurements.size())
    return;

  gettimeofday( &( m_vMeasurements[ cls ].startTime ), NULL );
}


// --------------------------------------------------------------------------
void TimeTracker::pingEnd( unsigned int cls )
// --------------------------------------------------------------------------
{
  stop( cls, true );
}


// --------------------------------------------------------------------------
void TimeTracker::stop
    ( unsigned int cls, const bool withStdDeviation, const double outlier )
// --------------------------------------------------------------------------
{
  if ( cls >= m_vMeasurements.size() )
    return;

  gettimeofday( &current_time, NULL );

  const timeval & t2 = current_time;
  const timeval & t1 = m_vMeasurements[ cls ].startTime;

  long sec  = t2.tv_sec  - t1.tv_sec;
  long usec = t2.tv_usec - t1.tv_usec;

  if ( usec < 0 )
  {
    sec  -= 1;
    usec += 1000000;
  }

  double recent = (double) ( sec + usec / 1000000.0 );

  // Do not store outliers
  if ( recent > outlier )
    return;

  m_vMeasurements[ cls ].recent = recent;

  if ( recent < m_vMeasurements[ cls ].minimum )
    m_vMeasurements[ cls ].minimum = recent;

  if ( recent > m_vMeasurements[ cls ].maximum )
    m_vMeasurements[ cls ].maximum = recent;

  // Store measured time if std. deviation is to be computed
  if ( withStdDeviation )
  {
    m_vMeasurements[ cls ].times.push_back( recent );
  }

  // Compute average on the fly
  double avg = m_vMeasurements[ cls ].average;
  avg *= m_vMeasurements[ cls ].count;
  avg += recent;
  ++m_vMeasurements[ cls ].count;
  avg /= m_vMeasurements[ cls ].count;
  m_vMeasurements[ cls ].average = avg;
}


// --------------------------------------------------------------------------
double TimeTracker::
      getStandardDeviation( unsigned int cls ) const
// --------------------------------------------------------------------------
{
  MeasurementClass * mc = (MeasurementClass*) &( m_vMeasurements[ cls ] );
  uint n = mc->times.size();

  if ( n < 2 )
  {
    cerr << "WARNING: Too few measurements for class " << cls << endl;
    return 0.0;
  }

  double sqr_dev = 0.0;
  double avg = 0.0;

  // Compute average
  for ( uint i = 0; i < n; ++i )
  {
    avg += mc->times[i];
  }
  avg /= n;

  // Compute squared deviations
  for ( uint i = 0; i < n; ++i )
  {
    sqr_dev += sqr( mc->times[i] - avg );
  }

  // Compute std. deviation
  return sqrt( sqr_dev / (n-1) );
}


// --------------------------------------------------------------------------
double TimeTracker::
      getElapsedTime( unsigned int cls )
// --------------------------------------------------------------------------
{
  if ( cls >= m_vMeasurements.size() )
    return 0.0;

  gettimeofday( &current_time, NULL);

  const timeval & t2 = current_time;
  const timeval & t1 = m_vMeasurements[ cls ].startTime;

  long sec  = t2.tv_sec  - t1.tv_sec;
  long usec = t2.tv_usec - t1.tv_usec;

  if ( usec < 0 )
  {
    sec  -= 1;
    usec += 1000000;
  }

  return sec + usec / 1000000.0;
}


// --------------------------------------------------------------------------
void TimeTracker::printToStdout( bool verbose )
// --------------------------------------------------------------------------
{
  printToStream( cout, verbose );
}


// --------------------------------------------------------------------------
void TimeTracker::printToStream( ostream & s, bool verbose )
// --------------------------------------------------------------------------
{
  for ( uint i = 0; i < m_vMeasurements.size(); ++i )
  {
    MeasurementClass& it = m_vMeasurements[i];

    s << "  Times of '" << setw( m_iMaxClassNameLength ) << left << it.name << right << "':    avg=" << it.average*1000 << " ms,  recent=" << it.recent*1000;
    s << " ms, min=" << it.minimum*1000 << " ms, max=" << it.maximum*1000 << "ms (in " << it.count << " calls)";

    if ( verbose )
    {
      s << ", stdev=" << (getStandardDeviation( i )*1000) << " ms";
    }

    s << '\n';
  }
}
