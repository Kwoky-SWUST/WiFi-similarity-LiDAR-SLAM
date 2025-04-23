#include "running_mean_and_std_dev.h"
#include <cmath>


// --------------------------------------------------------------------------
RunningMeanAndStandardDeviation::
RunningMeanAndStandardDeviation()
// --------------------------------------------------------------------------
{
  sumOfValues = 0;
  sumOfSquaredValues = 0;
  countValues = 0;
  minValue = INFINITY;
  maxValue = -INFINITY;
}


// --------------------------------------------------------------------------
void RunningMeanAndStandardDeviation::
    reset()
// --------------------------------------------------------------------------
{
  sumOfValues = 0;
  sumOfSquaredValues = 0;
  countValues = 0;
  minValue = INFINITY;
  maxValue = -INFINITY;
}


// --------------------------------------------------------------------------
void RunningMeanAndStandardDeviation::
    add( double value )
// --------------------------------------------------------------------------
{
  sumOfValues += value;
  sumOfSquaredValues += ( value*value );
  if ( value > maxValue )
    maxValue = value;
  if ( value < minValue )
    minValue = value;

  ++countValues;
}


// --------------------------------------------------------------------------
void RunningMeanAndStandardDeviation::
    set( std::vector< double >& values )
// --------------------------------------------------------------------------
{
  sumOfValues = sumOfSquaredValues = 0;
  for ( uint u = 0; u < values.size(); ++u )
  {
    sumOfValues += values[u];
    sumOfSquaredValues += ( values[u]*values[u] );

    if ( values[u] > maxValue )
      maxValue = values[u];
    if ( values[u] < minValue )
      minValue = values[u];
  }
  countValues = values.size();
}


// --------------------------------------------------------------------------
double RunningMeanAndStandardDeviation::
    getMean() const
// --------------------------------------------------------------------------
{
  return sumOfValues / countValues;
}


// --------------------------------------------------------------------------
double RunningMeanAndStandardDeviation::
      getVariance() const
// --------------------------------------------------------------------------
{
  /// @see http://de.wikipedia.org/w/index.php?title=Standardabweichung&oldid=75864105
  return ( sumOfSquaredValues - (sumOfValues*sumOfValues) / countValues ) / ( countValues - 1 );
}


// --------------------------------------------------------------------------
double RunningMeanAndStandardDeviation::
      getStandardDeviation() const
// --------------------------------------------------------------------------
{
  return sqrt( getVariance() );
}


// --------------------------------------------------------------------------
double RunningMeanAndStandardDeviation::
      getSumOfValues() const
// --------------------------------------------------------------------------
{
  return sumOfValues;
}


// --------------------------------------------------------------------------
double RunningMeanAndStandardDeviation::
      getMaximum() const
// --------------------------------------------------------------------------
{
  return maxValue;
}


// --------------------------------------------------------------------------
double RunningMeanAndStandardDeviation::
      getMinimum() const
// --------------------------------------------------------------------------
{
  return minValue;
}

