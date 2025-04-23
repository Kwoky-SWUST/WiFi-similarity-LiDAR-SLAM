#ifndef SCIROCO__UTILS__RUNNING_MEAN_AND_STD_DEV_H
#define SCIROCO__UTILS__RUNNING_MEAN_AND_STD_DEV_H

#include <stdlib.h>
#include <vector>

/** This class eases the computation of running means
  *  @author Philipp Vorst
  */
// --------------------------------------------------------------------------
class RunningMeanAndStandardDeviation
// --------------------------------------------------------------------------
{
  public:
    /** Constructor */
    RunningMeanAndStandardDeviation();

    void reset();

    /** Add a new measurement */
    void add( double value );

    /** Replace the statistics by the given vector of values */
    void set( std::vector< double >& values );

    /** Get the current mean */
    double getMean() const;

    /** Get the current variance */
    double getVariance() const;

    /** Get the current standard deviation */
    double getStandardDeviation() const;

    /** Get the current sum of all values */
    double getSumOfValues() const;

    /** Get the current maximum of all values */
    double getMaximum() const;

    /** Get the current minimum of all values */
    double getMinimum() const;

    /** Get the number of averaged, processed input values */
    uint   getNumberOfValues() const { return countValues; }

  protected:
    double sumOfValues;
    double sumOfSquaredValues;
    uint   countValues;
    double maxValue;
    double minValue;
};


#endif // SCIROCO__UTILS__RUNNING_MEAN_AND_STD_DEV_H
