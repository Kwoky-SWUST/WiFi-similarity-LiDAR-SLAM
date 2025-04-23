#ifndef SCIROCO__UTILS__QUALITY_CHECKS_H
#define SCIROCO__UTILS__QUALITY_CHECKS_H

#include <assert.h>


/// Toggle this define statement to enable/disable quality checks
#define ENABLE_QUALITY_CHECKS

#ifdef ENABLE_QUALITY_CHECKS
  #define QUALITY_CHECK if(1)
  #define ASSERT(x) assert(x)
#else
  #define QUALITY_CHECK if(0)
  #define ASSERT(x) {}
#endif



#endif // SCIROCO__UTILS__QUALITY_CHECKS_H
