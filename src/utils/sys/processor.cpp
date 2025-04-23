#include "processor.h"
#include <utils/text/to_string.h>
//
#include <iostream>
#include <fstream>
//
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>  // for getpid()
#include <unistd.h>     // for getpid()

using namespace std;


// --------------------------------------------------------------------------
std::string getProcInfo( const std::string & property, const char separator = 0 )
// --------------------------------------------------------------------------
{
  string proc_model;

  string command_str = "cat /proc/cpuinfo | grep " + property + " | head -n 1";
  const char* mode = "r";
  FILE * pipe_fd = popen( command_str.c_str(), mode );

  if ( pipe_fd != NULL )
  {
    char * line = NULL;
    size_t len = 0;
    ssize_t read = getline(&line, &len, pipe_fd);
    if ( read != -1 )
    {
      proc_model = string( line );
      size_t separator_index;
      if ( separator > 0 )
        separator_index = proc_model.find_first_of( ':' );
      else
        separator_index = 0;

      if ( separator_index != string::npos && ( separator_index == 0 || separator_index < proc_model.length() - 1 ) )
      {
        if ( separator_index > 0 )
          separator_index = proc_model.find_first_not_of( ' ', separator_index + 1 );
        size_t line_end_index = proc_model.find_first_of( '\n' );
        if ( line_end_index != string::npos )
          proc_model = proc_model.substr( separator_index, line_end_index - separator_index );
        else
          proc_model = proc_model.substr( separator_index );
      }
    }
    if ( line )
      free( line );
    pclose( pipe_fd );
  }
  else
  {
    switch ( errno )
    {
      case EINVAL:
        cout << "ERROR opening pipe: Invalid mode" << endl;
        break;
      case EMFILE:
        cout << "ERROR opening pipe: {FOPEN_MAX} or {STREAM_MAX} streams are currently open in the calling process." << endl;
        break;
      default:
        cout << "ERROR opening pipe: Unknown reason" << endl;
        break;
    }
  }

  return proc_model;
}


// --------------------------------------------------------------------------
std::string getProcessorModelInfo()
// --------------------------------------------------------------------------
{
  return getProcInfo( "\"model name\"", ':' );
}


// --------------------------------------------------------------------------
std::string getCPUMHz()
// --------------------------------------------------------------------------
{
  return getProcInfo( "\"cpu MHz\"", ':' );
}


// --------------------------------------------------------------------------
std::string getNumberOfProcessors()
// --------------------------------------------------------------------------
{
  return getProcInfo( "-c \"processor\"" );
}

