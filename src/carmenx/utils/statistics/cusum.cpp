#include <math.h>
#include <iostream>
#include <algorithm>
#include <sstream>

#include <utils/gnuplot/gnuplot.h>

#include "cusum.h"


// --------------------------------------------------------------------------
CumulativeSumWatcher::CumulativeSumWatcher( const CuSumConfig& cfg )
        : m_config(cfg),
          m_pPlotter(NULL)
// --------------------------------------------------------------------------
{
  initialize();
}

// --------------------------------------------------------------------------
CumulativeSumWatcher::CumulativeSumWatcher( double mean )
        : m_pPlotter(NULL)
// --------------------------------------------------------------------------
{
  m_config.mean = mean;
  initialize();
}


// --------------------------------------------------------------------------
CumulativeSumWatcher::~CumulativeSumWatcher()
// --------------------------------------------------------------------------
{
  reset();
}


// --------------------------------------------------------------------------
void CumulativeSumWatcher::initialize()
// --------------------------------------------------------------------------
{
  m_vCUSUMSamples.push_back( CuSumSample( 0.,0.,0.,0. ) );
  delete m_pPlotter;
  m_pPlotter = new GnuplotInterface();
}


// --------------------------------------------------------------------------
void CumulativeSumWatcher::reset()
// --------------------------------------------------------------------------
{
  delete m_pPlotter;
  m_pPlotter = NULL;
}

// --------------------------------------------------------------------------
CuSumAlarmStatus CumulativeSumWatcher::addSample( const double s )
// --------------------------------------------------------------------------
{
  CuSumSample new_sample;

  const CuSumSample& last_sample = m_vCUSUMSamples.back();
  new_sample.sdiff = s - m_config.mean;
  new_sample.s = last_sample.s + new_sample.sdiff;
  new_sample.sminus = std::max( 0., last_sample.sminus - new_sample.sdiff );
  new_sample.splus  = std::max( 0., last_sample.splus  + new_sample.sdiff );

  m_vCUSUMSamples.push_back( new_sample );

  if(m_config.bPlottingEnabled)
    plot();

  return CUSUM_STATUS_OK;
}

// --------------------------------------------------------------------------
void CumulativeSumWatcher::plot( const std::string& title /* = std::string("") */ ) const
// --------------------------------------------------------------------------
{
  if(m_pPlotter == NULL) {
    return;
  }

  std::stringstream str, data_s, data_sm, data_sp;
  str << "set title 'CUSUM" << (title.empty() ? "" : " (" + title + ")") << "'\nset xlabel 'sample'\n";
  str << "plot '-' with lines ti 'S', '-' with lines ti 'S+', '-' with lines ti 'S-'\n";
  for ( int i = 0; i < m_vCUSUMSamples.size(); ++i )
  {
    data_s << i << ' ' << m_vCUSUMSamples[i].s << '\n';
    data_sp << i << ' ' << m_vCUSUMSamples[i].splus << '\n';
    data_sm << i << ' ' << m_vCUSUMSamples[i].sminus << '\n';
  }
  data_s << "e\n";
  data_sp << "e\n";
  data_sm << "e\n";

  m_pPlotter->commandStr( str.str() + data_s.str() + data_sp.str() + data_sm.str() );
}


// --------------------------------------------------------------------------
const CuSumConfig CumulativeSumWatcher::getConfig() const
// --------------------------------------------------------------------------
{
  return m_config;
}

// --------------------------------------------------------------------------
void CumulativeSumWatcher::setConfig(const CuSumConfig& config)
// --------------------------------------------------------------------------
{
  m_config = config;
}


// --------------------------------------------------------------------------
const std::vector<double> CumulativeSumWatcher::getRawSamples() const
// --------------------------------------------------------------------------
{
  return m_vRawSamples;
}

