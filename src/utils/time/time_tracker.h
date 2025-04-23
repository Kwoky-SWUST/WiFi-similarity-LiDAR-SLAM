#ifndef CROCO__UTILS__TIME_TRACKER_H
#define CROCO__UTILS__TIME_TRACKER_H

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <sys/time.h>

/** Time tracking utility. Note that the interface is similar to the RCSoft5
 *  TimeTracker written by Tim Niemueller, but the backend is a lightweight
 *  one suitable for frequent measurements and output of average times.
 *  Deviations are not recorded.
 *
 *  @author Tim Niemueller (original interface), Philipp Vorst for new backend
 */
// --------------------------------------------------------------------------
class TimeTracker
// --------------------------------------------------------------------------
{
  public:
    /** Default constructor */
    TimeTracker();

    /** Default destructor */
    ~TimeTracker();

  public:
    static TimeTracker * getInstance();

    /** Add a measurement class with the given name.
     *  @return A number which uniquely identifies the new class.
     */
    unsigned int addClass( const std::string & name );

    /** @deprecated */
    void pingStart   ( unsigned int cls = 0 );
    /** @deprecated */
    void pingEnd     ( unsigned int cls = 0 );

    /** Start a measurement for the given class number. */
    void start       ( unsigned int cls = 0 );
    /** Stop the recent measurement for the given class number. */
    void stop        ( unsigned int cls = 0, const bool withStdDeviation = false,
                       const double outlier = 1000000.0f );

    /** Get the elapsed time for the given class since the last call to
      * start. */
    double getElapsedTime( unsigned int cls );

    /** Clear all values */
    void reset();

    /** Print time measurements to standard output.
     ** @param verbose If true, deviations are printed, too. */
    void printToStdout( bool verbose = false );

    /** Print time measurements to the given stream.
     ** @param verbose If true, deviations are printed, too. */
    void printToStream( std::ostream & s, bool verbose = false );

    /** Get the standard deviation of time for a given class */
    double getStandardDeviation( unsigned int cls ) const;


  private:
    /** Struct for recording the measurements assigned to one class */
    struct MeasurementClass
    {
      std::string           name;
      double                average;
      double                recent;
      double                minimum;
      double                maximum;
      unsigned int          count;
      timeval               startTime;
      timeval               stopTime;
      std::vector< double > times;
    };

    /** The list of measurements */
    std::vector< MeasurementClass > m_vMeasurements;

    /** Temporary variable for intermediate time stamps */
    timeval current_time;

    /** The length of the longest class name */
    int m_iMaxClassNameLength;
};


#endif // CROCO__UTILS__TIME_TRACKER_H
