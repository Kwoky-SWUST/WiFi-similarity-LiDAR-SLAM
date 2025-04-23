#ifndef SCIROCO__UTILS__STATISTICS_CUSUM_H_1131239890
#define SCIROCO__UTILS__STATISTICS_CUSUM_H_1131239890

#include <vector>
#include <string>

class GnuplotInterface;

struct CuSumConfig {
    CuSumConfig() : mean(0), bPlottingEnabled(false) {}
    CuSumConfig( const CuSumConfig& c ) : mean(c.mean), bPlottingEnabled(c.bPlottingEnabled) {}
    ~CuSumConfig() {}

    double mean;
    bool bPlottingEnabled;
};

enum CuSumAlarmStatus {
  CUSUM_STATUS_OK              = 0,
  CUSUM_STATUS_RED_ALARM       = 1
};

struct CuSumSample {
    CuSumSample() {}
    CuSumSample(double _s, double _sdiff, double _sminus, double _splus)
        : s(_s), sdiff(_sdiff), sminus(_sminus), splus(_splus) {}
    ~CuSumSample() {}

    double s;
    double sdiff;
    double sminus;
    double splus;
};


class CumulativeSumWatcher
{
  public:
    /// CTOR/DTOR
    CumulativeSumWatcher( const CuSumConfig& cfg );
    CumulativeSumWatcher( double mean );
    ~CumulativeSumWatcher();

  protected:
    void reset();
    void initialize();

  public:
    CuSumAlarmStatus addSample( const double s );
    void plot( const std::string& title = std::string("") ) const;

    const CuSumConfig getConfig() const;
    void setConfig( const CuSumConfig& config );
    const std::vector<double> getRawSamples() const;

    /// DATA:
    std::vector<double>         m_vRawSamples;
    std::vector<CuSumSample>    m_vCUSUMSamples;
    CuSumConfig                 m_config;
    GnuplotInterface*           m_pPlotter;
};





#endif
