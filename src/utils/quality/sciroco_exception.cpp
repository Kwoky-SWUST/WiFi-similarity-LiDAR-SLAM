#include "sciroco_exception.h"
#include <utils/text/to_string.h>

using namespace std;


// --------------------------------------------------------------------------
ScirocoException::
ScirocoException( ScirocoExceptionType type )
// --------------------------------------------------------------------------
{
  m_iType = type;
  m_sErrorMessage = "";
}


// --------------------------------------------------------------------------
ScirocoException::
ScirocoException( const std::string errorMessage,
                  ScirocoExceptionType type )
// --------------------------------------------------------------------------
{
  m_iType = type;
  m_sErrorMessage = errorMessage;
}


// --------------------------------------------------------------------------
ScirocoException::
ScirocoException( const std::string     errorMessage,
                  std::string           functionName,
                  unsigned int          lineNumber,
                  ScirocoExceptionType  type )
// --------------------------------------------------------------------------
{
  m_iType = type;
  m_sErrorMessage = errorMessage + getDescriptionOfOrigin( functionName, lineNumber );
}


// --------------------------------------------------------------------------
ScirocoException::
ScirocoException( const char* errorMessage,
                  ScirocoExceptionType type )
// --------------------------------------------------------------------------
{
  if ( errorMessage == NULL )
    throw ScirocoException( "Call to ScirocoException with NULL error message", INTERNAL_ERROR );

  m_iType = type;
  m_sErrorMessage = errorMessage;
}


// --------------------------------------------------------------------------
std::string ScirocoException::
    getMessage() const
// --------------------------------------------------------------------------
{
  return m_sErrorMessage;
}


// --------------------------------------------------------------------------
ScirocoException::ScirocoExceptionType ScirocoException::
    getType() const
// --------------------------------------------------------------------------
{
  return m_iType;
}


// --------------------------------------------------------------------------
std::string ScirocoException::
getDescriptionOfOrigin( std::string   functionName,
                        unsigned int  lineNumber )
// --------------------------------------------------------------------------
{
  string s;
  if ( functionName != "" || lineNumber > 0 )
  {
    s += " (";
    if ( functionName != "" )
    {
      s += functionName;
      if ( lineNumber > 0 )
        s += ":";
    }
    if ( lineNumber > 0 )
      s += "line " + toString( lineNumber );
    s += ")";
  }
  return s;
}


// --------------------------------------------------------------------------
void ScirocoException::
testAllocation( void*         pointer,
                std::string   failureMessage,
                std::string   functionName,
                int           lineNumber  )
// --------------------------------------------------------------------------
{
  if ( pointer == NULL )
  {
    std::string s( "Failed to allocate memory" );
    if ( failureMessage != "" )
      s += ": " + failureMessage;
    s += getDescriptionOfOrigin( functionName, lineNumber );
    ScirocoException exception( s );
    throw exception;
  }
}


// --------------------------------------------------------------------------
void ScirocoException::
test( bool          condition,
      std::string   failureMessage,
      std::string   functionName,
      unsigned int  lineNumber  )
// --------------------------------------------------------------------------
{
  if ( ! condition )
  {
    std::string s( failureMessage );
    s += getDescriptionOfOrigin( functionName, lineNumber );
    ScirocoException exception( s );
    throw exception;
  }
}
