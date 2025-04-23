#ifndef UTILS_GET_LINE_BY_STREAM
#define UTILS_GET_LINE_BY_STREAM

#include <iostream>
#include <sstream>


/** This function reads a line from an ifstream, turns it into the string
 *  'line' and additionally create an istringstream from it.
 */
// --------------------------------------------------------------------------
inline void
    get_line_by_stream( std::ifstream&       istr,
                        std::istringstream&  iss_line,
                        std::string&         line )
// --------------------------------------------------------------------------
{
  iss_line.clear();
  getline( istr, line );
  iss_line.str( line );
}

#endif // UTILS_GET_LINE_BY_STREAM
