#ifndef __RCA_SERIAL_INTERFACE_CPP
#define __RCA_SERIAL_INTERFACE_CPP

#include <utils/serial/serial_interface.h>
//
#include <sys/time.h>

using namespace std;

//#define DEBUG if(0) cout
#define DEBUG cout


// --------------------------------------------------------------------------
SerialInterface::SerialInterface( const string & device )
// --------------------------------------------------------------------------
{
  m_bConnected = false;
  m_sDeviceName = device;

  openDevice();
  configure_very_old();
  DEBUG << "Compile version: " << __TIME__ << endl;
}


// --------------------------------------------------------------------------
SerialInterface::~SerialInterface()
// --------------------------------------------------------------------------
{
  closeDevice();
}


// --------------------------------------------------------------------------
void SerialInterface::openDevice()
// --------------------------------------------------------------------------
{
  // Complain if device is already opened.
  if ( m_bConnected )
  {
    throw( SERIAL_INTERFACE_EXCEPTION_DEVICE_ALREADY_OPENED );
  }

  DEBUG << "Trying to create file descriptor for " << m_sDeviceName.c_str() << "..." << endl;
//   m_fileDescriptor = open( m_sDeviceName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY );
// before Alien:

//   m_fileDescriptor = open( m_sDeviceName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY ); // Sebastian's version
  m_fileDescriptor = open( m_sDeviceName.c_str(), O_RDWR | O_NOCTTY | O_EXCL | O_NDELAY );
  DEBUG << "File Descriptor: " << m_fileDescriptor << endl;

  // Is descriptor valid?
  if ( m_fileDescriptor == -1 )
  {
    perror( "SerialInterface::openDevice" );

    // No, so throw exception
    throw( SERIAL_INTERFACE_EXCEPTION_ON_OPEN );
  }
  else
  {
//     fcntl( m_fileDescriptor, F_SETFL, FNDELAY ); // FNDELAY : Wenn keine Daten verf&uuml;gbar sind,
                  //           soll 'read' 0 zur&uuml;ckliefern
    m_bConnected = true;
  }
}


// --------------------------------------------------------------------------
void SerialInterface::closeDevice()
// --------------------------------------------------------------------------
{
  if ( m_bConnected )
  {
    close( m_fileDescriptor );  // close device file
    m_bConnected = false;
  }
  else
  {
    throw( SERIAL_INTERFACE_EXCEPTION_DEVICE_NOT_OPENED );
  }
}


// --------------------------------------------------------------------------
void SerialInterface::checkDevice()
// --------------------------------------------------------------------------
{
  if ( ! m_bConnected )
  {
    return;
  }

  // Update status information
  struct termios * info = getConfigurationStatus();

  DEBUG << "PARITY:       "   << ((info->c_cflag & PARENB) ? "YES" : "NO");
  DEBUG << "\n8 DATA BITS:  " << ((info->c_cflag & CS8) ? "YES" : "NO");
  DEBUG << "\nREADY:        " << ((info->c_cflag & CREAD) ? "YES" : "NO");
  DEBUG << "\nINPUT SPEED:  " << info->c_ispeed;
  DEBUG << "\nOUTPUT SPEED: " << info->c_ospeed;
  DEBUG << "\nSPEED:        " << ( info->c_cflag & CBAUD ) << endl;
}


// --------------------------------------------------------------------------
void SerialInterface::configure()
// --------------------------------------------------------------------------
{
  termios options;
  tcgetattr( m_fileDescriptor, &options );
  termios oldOptions = options;

  options.c_cflag |= (CLOCAL | CREAD);

  // set baud rate
  cfsetispeed(&options, B9600 );
  cfsetospeed(&options, B9600 );

  // set 8 bit, 1 stop bit, no parity
  options.c_cflag &= ~PARENB;
  options.c_cflag &= ~CSTOPB;
  options.c_cflag &= ~CSIZE;
  options.c_cflag |= CS8;

  // set no hardware flow control (if supported by system)
  //options.c_cflag &= ~CNEW_RTSCTS;

  // set raw input
//     options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

  // set waiting conditions for read
  options.c_cc[VMIN] = 0;
  options.c_cc[VTIME] = 1;

  tcsetattr(m_fileDescriptor, TCSANOW, &options);

  fcntl( m_fileDescriptor, F_SETFL, O_RDONLY );
}


// --------------------------------------------------------------------------
void SerialInterface::configure_medium_old()
// --------------------------------------------------------------------------
{
  termios options;
  tcgetattr( m_fileDescriptor, &options );
  termios oldOptions = options;

  options.c_cflag |= (CLOCAL | CREAD);

  // set baud rate
  cfsetispeed(&options, B115200);
  cfsetospeed(&options, B115200);

  // set 8 bit, 1 stop bit, no parity
  options.c_cflag &= ~PARENB;
  options.c_cflag &= ~CSTOPB;
  options.c_cflag &= ~CSIZE;
  options.c_cflag |= CS8;

  // set no hardware flow control (if supported by system)
  //options.c_cflag &= ~CNEW_RTSCTS;

  // set raw input
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

  // set waiting conditions for read
  options.c_cc[VMIN] = 0;
  options.c_cc[VTIME] = 1;

  tcsetattr(m_fileDescriptor, TCSANOW, &options);

  fcntl(m_fileDescriptor, F_SETFL, 0);

  cerr << "Options set" << endl;
}


// --------------------------------------------------------------------------
void SerialInterface::configure_very_old()
// --------------------------------------------------------------------------
{
  struct termios options;
  int reply, status;

  // Alte Porteinstellungen lesen
  DEBUG << "Lese alte Porteinstellungen..." << endl;

  reply = tcgetattr( m_fileDescriptor, &options); // Alte Porteinstellungen einlesen

  DEBUG << "Alte Einstellungen: iflag = " << options.c_iflag << ", oflag = " << options.c_oflag << ", cflag = " << options.c_cflag << ",lflag = " << options.c_lflag << endl;

  options.c_cflag &= ~CBAUD; // Baudrate nullen...
  options.c_cflag |= CLOCAL;  // Nicht den Besitz des Ports &uuml;bernehmen, sei ANGEBLICH unn&ouml;tig
  options.c_cflag |= CREAD;   // Empfangsbereitschaft am Port herstellen

  DEBUG << "Setze Eingangs-/Ausgangsbaudrate auf 115200 Baud (default)..." << endl;
  reply = cfsetispeed( &options, BAUD_RATE_115200 );
  reply = cfsetospeed( &options, BAUD_RATE_115200 );

  DEBUG << "Stelle Parameter 8N1 usw. ein..." << endl;

  options.c_cflag &= ~PARENB; // no parity
  options.c_cflag &= ~CSTOPB; // 1 stop bit
  options.c_cflag &= ~CSIZE;  // Alte Datenbitangabe l&ouml;schen
  options.c_cflag |= CS8;     // 8 Datenbits

  options.c_cflag &= ~CRTSCTS;// no flow control
  options.c_cflag &= ~HUPCL;  // no hang up if process is terminated

  options.c_lflag &= ~ISIG;   // Keine Signale zulassen (Is ja kein echtes TTY dran)
  options.c_lflag &= ~ICANON; // RAW-Modus
  options.c_lflag &= ~ECHO;   // no echo
  options.c_lflag &= ~ECHOE;  // Erst recht nicht f&uuml;r den Backspace-Character
  options.c_lflag &= ~(XCASE | ECHOK | ECHONL | NOFLSH | IEXTEN | ECHOCTL | ECHOPRT | ECHOKE | FLUSHO | PENDIN | TOSTOP); // ??

  options.c_iflag &= ~INPCK;  // no parity check
  options.c_iflag |= IGNPAR;  // Deshalb auch Parit&auml;tsfehler ignorieren.
  options.c_iflag &= ~(IXOFF | IXON | IXANY); // Kein Software-Handshaking
  options.c_iflag |= IGNBRK; // Verbindungstrennung ignorieren
  options.c_iflag &= ~(INLCR | IGNCR | ICRNL | IUCLC); // Keine dummen Spielchen mit meinen Zeichen machen

  options.c_oflag &= ~OPOST; // Kein Postprocessing
  options.c_oflag &= ~ONLCR; // ** zur sicherheit

  // set waiting conditions for read
  options.c_cc[ VMIN ] = 0;
  options.c_cc[ VTIME ] = 1;

  // options.c_cc[VMIN] = 1; // read soll Blocken bis ein Zeichen Gelesen wurde.
  reply = tcsetattr( m_fileDescriptor, TCSANOW, &options );
  if ( reply == -1 )
  {
    perror( __FUNCTION__ );
  }

//   fcntl( m_fileDescriptor, F_SETFL, 0 );

  reply = ioctl( m_fileDescriptor, TIOCMGET, &status ); // Alten status abgreifen

  status |= TIOCM_DTR;
  status &= ~TIOCM_RTS;

  reply = ioctl( m_fileDescriptor, TIOCMSET, &status );

  DEBUG << "OK." << endl;
}


// --------------------------------------------------------------------------
void SerialInterface::flush()
// --------------------------------------------------------------------------
{
  tcflush( m_fileDescriptor, TCIOFLUSH );
}


// --------------------------------------------------------------------------
struct termios* SerialInterface::getConfigurationStatus()
// --------------------------------------------------------------------------
{
  // Store device configuration in member struct named "m_configStatus"
  tcgetattr( m_fileDescriptor, &m_configStatus );

  return &m_configStatus;
}


// --------------------------------------------------------------------------
void SerialInterface::setBaudRate( int baudRate )
// --------------------------------------------------------------------------
{
//   if ( (baudRate & CBAUD) == 0 )
//   {
//     throw( SERIAL_INTERFACE_EXCEPTION_INVALID_BAUD_RATE );
//   }

  int _reply;
  getConfigurationStatus();

  speed_t baudRateConstant =
    (baudRate == 0) ? B0 :
    (baudRate == 50) ? B50 :
    (baudRate == 75) ? B75 :
    (baudRate == 110) ? B110 :
    (baudRate == 134) ? B134 :
    (baudRate == 150) ? B150 :
    (baudRate == 200) ? B200 :
    (baudRate == 300) ? B300 :
    (baudRate == 600) ? B600 :
    (baudRate == 1200) ? B1200 :
    (baudRate == 1800) ? B1800 :
    (baudRate == 2400) ? B2400 :
    (baudRate == 4800) ? B4800 :
    (baudRate == 9600) ? B9600 :
    (baudRate == 19200) ? B19200 :
    (baudRate == 38400) ? B38400 :
    (baudRate == 57600) ? B57600 :
    (baudRate == 115200) ? B115200 :
    (baudRate == 230400) ? B230400 : B0;


  _reply = cfsetispeed( &m_configStatus, baudRateConstant );
  _reply = cfsetospeed( &m_configStatus, baudRateConstant );
  _reply = tcsetattr( m_fileDescriptor, TCSANOW, &m_configStatus );

  /// @TODO: _reply auswerten
}


// --------------------------------------------------------------------------
char SerialInterface::readByte()
// --------------------------------------------------------------------------
{
  char data;
  int reply = read( m_fileDescriptor, &data, 1 );  // Read exactly one byte

  if ( reply <= 0 )
  {
    throw( SERIAL_INTERFACE_EXCEPTION_NOTHING_READ );
  }

  return data;
}


// --------------------------------------------------------------------------
int SerialInterface::readBytes( const int bufferSize, char * buffer,
                                const float timeout )
// --------------------------------------------------------------------------
{
  int bytes_read = 0;
  int bytes_read_total = 0;
  int remaining_buffer_size = bufferSize;
  char * original_buffer_start = buffer;

  string input_string("");

  struct timeval tv;
  int err = gettimeofday( &tv, NULL );
  if ( err == -1 )
    perror( "gettimeofday" );
  double start_time = tv.tv_sec + tv.tv_usec/1000000.0;

  double current_time;
  do
  {
    // Actual reading and byte counting
    bytes_read = read( m_fileDescriptor, buffer, remaining_buffer_size );

    if ( bytes_read > 0 )
    {

      bytes_read_total      += bytes_read;
      buffer                += bytes_read;
      remaining_buffer_size -= bytes_read;

//       cout << "Intermediary length: " << bytes_read_total << ", input_string: \n#" << input_string << "#" << endl;
    }

    int err = gettimeofday( &tv, NULL );
    if ( err == -1 )
      perror( "gettimeofday" );
    current_time = tv.tv_sec + tv.tv_usec/1000000.0;
    
    usleep( 10 );

    // Continue reading if
    //  a) nothing was read,
    //  b) the input string has not been terminated,
    //  c) the buffer size has not been reached,
    //  d) and reading has not timed out
  } while ( ( bytes_read == 0
            || *(buffer-1) != '\0' )
            && ( bytes_read_total < bufferSize )
            && ( timeout <= 0.0 || current_time-start_time < timeout )
          );

  // Copy buffer as a whole to string
  input_string.append( original_buffer_start, bytes_read_total );

  if ( timeout > 0.0 && current_time-start_time > timeout )
    cerr << "WARNING: Serial timeout: " << current_time-start_time << "\n";

//   cout << "Leaving readBytes()" << endl;

  return bytes_read_total;
}


// --------------------------------------------------------------------------
void SerialInterface::writeByte( char data )
// --------------------------------------------------------------------------
{
  int _reply = write( m_fileDescriptor, &data, 1 );
  if ( _reply <= 0 )
  {
    perror( "writeByte" );
    throw( SERIAL_INTERFACE_EXCEPTION_WRITE_ERROR );
  }
}


// --------------------------------------------------------------------------
void SerialInterface::writeBytes( char * data )
// --------------------------------------------------------------------------
{
  while ( *data )
  {
    writeByte( *data );
    ++data;
  }
}


// --------------------------------------------------------------------------
void SerialInterface::writeBytes( char * data, uint len )
// --------------------------------------------------------------------------
{
  for ( uint i = 0; i < len; ++i )
  {
    writeByte( *data );
    ++data;
  }
}


// --------------------------------------------------------------------------
int SerialInterface::bytesAvailable()
// --------------------------------------------------------------------------
{
  int bytesAvailable;
  ioctl( m_fileDescriptor, FIONREAD, &bytesAvailable );
  return bytesAvailable;
}

#endif
