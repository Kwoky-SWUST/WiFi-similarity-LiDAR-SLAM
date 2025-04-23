#include "mem.h"
#include <utils/text/to_string.h>
//
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <sys/types.h>  // for getpid()
#include <unistd.h>     // for getpid()

using namespace std;


// --------------------------------------------------------------------------
long getCurrentVmPeak()
// --------------------------------------------------------------------------
{
  long unsigned int vm_peak = 0;
  pid_t my_pid = getpid();

  string process_status_file( "/proc/" + toString( my_pid ) + "/status" );

  ifstream status_stream;
  status_stream.open( process_status_file.c_str(), ios::in );
  if ( ! status_stream )
  {
    cerr << "Error opening '" << process_status_file << "'" << endl;
  }
  else // Status file successfully opened
  {
    string  line;
    size_t  vm_peak_pos, kb_pos;
    string  search_for_vm_peak( "VmPeak:" );
    string  search_for_kb( "kB" );

    while ( getline( status_stream, line ) )
    {
      vm_peak_pos = line.find( search_for_vm_peak );
      kb_pos      = line.find( search_for_kb );
      if ( ( vm_peak_pos != string::npos ) && ( kb_pos != string::npos ) )
      {
        string value = line.substr( vm_peak_pos + search_for_vm_peak.length(), kb_pos - ( vm_peak_pos + search_for_vm_peak.length() ) );
        vm_peak = atol( value.c_str() );
      }
    }
  }

  if ( vm_peak == 0 )
  {
    cerr << "Warning: getCurrentVmPeak() was not able to retrieve VmPeak value" << endl;
  }

  return vm_peak;
}

