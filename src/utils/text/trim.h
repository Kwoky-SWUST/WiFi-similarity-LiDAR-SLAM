#ifndef CROCO__UTILS__TRIM
#define CROCO__UTILS__TRIM

#include <string>


// --------------------------------------------------------------------------
inline void trim( std::string & str )
// --------------------------------------------------------------------------
{
  std::string::size_type pos = str.find_last_not_of( ' ' );
  if ( pos != std::string::npos )
  {
    str.erase( pos + 1 );
    pos = str.find_first_not_of( ' ' );
    if ( pos != std::string::npos )
    {
      str.erase( 0, pos );
    }
  }
  else
  {
    str.erase( str.begin(), str.end() );
  }
}

#endif
