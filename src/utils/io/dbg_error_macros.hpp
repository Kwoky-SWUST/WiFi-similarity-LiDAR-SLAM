#ifndef DBG_ERR_MACROS_H_439589
#define DBG_ERR_MACROS_H_439589

// overwrite __FUNCTION__ macro if necessary
#ifndef _MSC_VER
#ifdef __GNUC__
#define __FUNCTION__ __PRETTY_FUNCTION__
#else
#define __FUNCTION__ "(unknown)"
#endif
#endif

/// define some other useful macros
#include <string>
#include <iostream>

#define FUNCTIONSTR std::string(__FUNCTION__)
#define ERROR_STR(x) FUNCTIONSTR + " -> " + (x)

// higher debugging level means more output
#ifndef DEBUG_OUT_LEVEL
#define DEBUG_OUT_LEVEL 0
#endif

#define DEBUG_IF(dbg_lvl) if( DEBUG_OUT_LEVEL >= dbg_lvl )
#define DEBUG_IF0 DEBUG_IF(0)
#define DEBUG_IF1 DEBUG_IF(1)
#define DEBUG_IF2 DEBUG_IF(2)
#define DEBUG_OUT(dbg_lvl) if( DEBUG_OUT_LEVEL >= dbg_lvl ) std::cout << std::flush
#define DEBUG_OUT_FUNCTION(dbg_lvl) if( DEBUG_OUT_LEVEL >= dbg_lvl ) std::cout << __FUNCTION__ << ": " << std::flush
#define DEBUG_OUT_FUNCTION_LINE(dbg_lvl) if( DEBUG_OUT_LEVEL >= dbg_lvl ) std::cout << __FUNCTION__ << " (line " << __LINE__ << "): " << std::flush
#define DEBUG_OUT_FILE_FUNCTION_LINE(dbg_lvl) if( DEBUG_OUT_LEVEL >= dbg_lvl ) std::cout << __FILE__ << " (" << __FUNCTION__ << ", line " << __LINE__ << "): " << std::flush
#define DEBUG_OUT0 DEBUG_OUT(0)
#define DEBUG_OUT1 DEBUG_OUT(1)
#define DEBUG_OUT2 DEBUG_OUT(2)
#define DEBUG_OUT_FUNCTION0 DEBUG_OUT_FUNCTION(0)
#define DEBUG_OUT_FUNCTION1 DEBUG_OUT_FUNCTION(1)
#define DEBUG_OUT_FUNCTION2 DEBUG_OUT_FUNCTION(2)
#define DEBUG_OUT_FUNCTION_LINE0 DEBUG_OUT_FUNCTION_LINE(0)
#define DEBUG_OUT_FUNCTION_LINE1 DEBUG_OUT_FUNCTION_LINE(1)
#define DEBUG_OUT_FUNCTION_LINE2 DEBUG_OUT_FUNCTION_LINE(2)
#define DEBUG_OUT_FILE_FUNCTION_LINE0 DEBUG_OUT_FILE_FUNCTION_LINE(0)
#define DEBUG_OUT_FILE_FUNCTION_LINE1 DEBUG_OUT_FILE_FUNCTION_LINE(1)
#define DEBUG_OUT_FILE_FUNCTION_LINE2 DEBUG_OUT_FILE_FUNCTION_LINE(2)
#define DEBUG_OUT_FUNCTION_TIME_LINE0 DEBUG_OUT_FUNCTION_LINE(0)
#define DEBUG_OUT_FUNCTION_TIME_LINE1 DEBUG_OUT_FUNCTION_LINE(1)
#define DEBUG_OUT_FUNCTION_TIME_LINE2 DEBUG_OUT_FUNCTION_LINE(2)

#endif // DBG_ERR_MACROS_H_439589
