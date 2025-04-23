#ifndef SCIROCO__UTILS__SCIROCO_EXCEPTION_H
#define SCIROCO__UTILS__SCIROCO_EXCEPTION_H

#include <string>


/** Base class of Sciroco exceptions
 *  @author Philipp Vorst
 */
// --------------------------------------------------------------------------
class ScirocoException
// --------------------------------------------------------------------------
{
  public:
    enum ScirocoExceptionType {
      ANY                           = 0,
      FILE_FORMAT_ERROR             = 1,
      INTERNAL_ERROR                = 2,
      FILE_READ_ERROR               = 3,
      FILE_OUTPUT_ERROR             = 4,
      NOT_YET_IMPLEMENTED           = 5,
      INVALID_COMMAND_LINE_ARGUMENT = 6,
      INVALID_ARGUMENT              = 7
    };

  public:
    /** Default constructor */
    ScirocoException( ScirocoExceptionType type = ANY );

    /** Constructor with a given error message and type */
    ScirocoException( const char* errorMessage, 
                      ScirocoExceptionType type = ANY );

    /** Constructor with a given error message and type */
    ScirocoException( const std::string errorMessage, 
                      ScirocoExceptionType type = ANY );

    /** Constructor with a given error message, type,
     *  and function name/line number of occurrence.
     */
    ScirocoException( const std::string    errorMessage, 
                      std::string          functionName,
                      unsigned int         lineNumber,
                      ScirocoExceptionType type = ANY );

    /** Test the given condition and throw an instance of ScirocoException 
     *  if the test fails.
     */
    static void test( bool                condition,
                      std::string         failureMessage,
                      std::string         functionName = "",
                      unsigned int        lineNumber = 0 );

    /** Test if memory has been allocated and throw an instance of 
     *  ScirocoException if the test fails.
     */
    static void testAllocation( void*               pointer,
                                std::string         failureMessage,
                                std::string         functionName = "",
                                int                 lineNumber = 0 );

    /** Return the associated error message of this exception */
    std::string getMessage() const;

    /** Return the type of exception */
    ScirocoExceptionType getType() const;


  protected:
    /** Turn the given information into a textual description */
    static std::string getDescriptionOfOrigin( std::string   functionName,
                                               unsigned int  lineNumber );

  protected:
    /** The associated error message of this exception */
    std::string m_sErrorMessage;

    /** The type of exception */
    ScirocoExceptionType m_iType;
};

#endif // SCIROCO__UTILS__SCIROCO_EXCEPTION_H
