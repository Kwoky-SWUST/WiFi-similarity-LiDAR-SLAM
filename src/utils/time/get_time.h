#ifndef SCIROCO__UTILS__GET_TIME_H
#define SCIROCO__UTILS__GET_TIME_H

#include <sys/time.h>
#include <string>

/** @author Philipp Vorst (DA) */


/** Get current time as timeval struct
 *  @see gettimeofday */
timeval & getCurrentTime();

/** Get the current number of seconds.
 *  @see gettimeofday */
long unsigned int getCurrentSeconds();

/** Get the current number of milliseconds of the current second.
 *  @see gettimeofday */
unsigned int getCurrentMilliSeconds();

/** Get the current number of microseconds of the current second.
 *  @see gettimeofday */
unsigned int getCurrentMicroSeconds();

/** Get the current date and time as a string */
std::string getDateAndTimeStamp();


#endif // SCIROCO__UTILS__GET_TIME_H
