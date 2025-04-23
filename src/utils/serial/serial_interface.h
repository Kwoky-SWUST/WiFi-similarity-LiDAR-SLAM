#ifndef __RCA_SERIAL_INTERFACE_H
#define __RCA_SERIAL_INTERFACE_H

#include <fcntl.h>  // file control
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>  // I/O control routines
#include <termios.h>  // POSIX terminal control
#include <unistd.h>
#include <sys/socket.h>
#include <asm/socket.h>
#include <errno.h>
#include <string>


/**
  * SerialInterface represents a simple interface for handling connections with
  * serial ports.
  *
  * The normal life cycle should look like as follows:
  *  - Construct the interface via SerialInterface( serialPortNumber )
  *  - openDevice()
  *  - configure()
  *  - setBaudRate()
  *  - Read & write, whatever you want... Note: Use bytesAvailable() before read operation.
  *  - closeDevice()
  *
  * @todo "Device" in "Port" umbenennen
  * @author Philipp & Metty(mean_hacker)
  * @version 1.0
  */
// --------------------------------------------------------------------------
class SerialInterface
// --------------------------------------------------------------------------
{
public:
  /** Create an interface for the given port number.
    * Note that, at this time, the
    * connection to the serial port is not established yet. Call openDevice() in
    * order to connect to the serial port.
    * Note further that instead of a concrete numeric value for portNumber you
    * should use one of the predefined constants SERIAL_x
    *
    * @param portNumber The number of the serial port (0-3)
    * @see openDevice
    * @see SERIAL_0, SERIAL_1, SERIAL_2, SERIAL_3
    */
  SerialInterface( const std::string & sDevice );

  /** Destroy the interface. */
  ~SerialInterface();


  // ------------------------------------------------------------------------
  // --------------------    C  O  N  S  T  A  N  T  S    -------------------
  // ------------------------------------------------------------------------

  /// Constant for a baud rate at 2400
  static const int BAUD_RATE_2400 = B2400;

  /// Constant for a baud rate at 4800
  static const int BAUD_RATE_4800 = B4800;

  /// Constant for a baud rate at 19200.
  static const int BAUD_RATE_19200 = B19200;

  /// Constant for a baud rate at 38400.
  static const int BAUD_RATE_38400 = B38400;

  /// Constant for a baud rate at 57600
  static const int BAUD_RATE_57600 = B57600;

  /// Constant for a baud rate at 115200
  static const int BAUD_RATE_115200 = B115200;

  /** Constant for exception handling.
    * This exception is thrown if the device cannot not be opened.
    * @see openDevice
    */
  static const int SERIAL_INTERFACE_EXCEPTION_ON_OPEN = 0;

  /** Constant for exception handling.
    * This exception is thrown when a device is to be opened though already opened before.
    * @see openDevice
    */
  static const int SERIAL_INTERFACE_EXCEPTION_DEVICE_ALREADY_OPENED = 1;

  /** Constant for exception handling.
    * This exception is thrown when a device is to be closed but has never been opened before.
    * @see closeDevice
    */
  static const int SERIAL_INTERFACE_EXCEPTION_DEVICE_NOT_OPENED = 2;

  /** Constant for exception handling.
    * This exception is thrown when the baud rate is set to an illegal value. Use the
    * predefined baud rate constants in order to avoid errors.
    * @see setBaudRate
    * @see BAUD_RATE_115200, BAUD_RATE_57600, BAUD_RATE_38400, BAUD_RATE_19200
    */
  static const int SERIAL_INTERFACE_EXCEPTION_INVALID_BAUD_RATE = 3;

  /** Constant for exception handling.
    * This exception is thrown when data is to be read but not present. Always consult
    * method bytesAvailable() before data is read in order to find out how many bytes
    * are waiting in the in-queue.
    * @see readByte, readBytes, bytesAvailable
    */
  static const int SERIAL_INTERFACE_EXCEPTION_NOTHING_READ = 4;

  /** Constant for exception handling.
    * when the desired write operation failed.
    * @see writeByte
    */
  static const int SERIAL_INTERFACE_EXCEPTION_WRITE_ERROR = 5;

  /** Synonym of specific port number 0. */
  static const int SERIAL_0 = 0;

  /** Synonym of specific port number 1. */
  static const int SERIAL_1 = 1;

  /** Synonym of specific port number 2. */
  static const int SERIAL_2 = 2;

  /** Synonym of specific port number 3. */
  static const int SERIAL_3 = 3;



  // ------------------------------------------------------------------------
  // ---------------------    M  E  T  H  O  D  S    ------------------------
  // ------------------------------------------------------------------------

  /** Configures the device properly. */
  void configure();
  void configure_medium_old();
  void configure_very_old();

  /** Flush input and output queues */
  void flush();

  /** Output information about device. */
  void checkDevice();

  /** Set the baud rate of the device. Note that this may only make sense if you do not call
    * method configure() later.
    */
  void setBaudRate( int baudRate );

  /** Read exactly one byte from device. */
  char readByte();

  /** Read a series of bytes
   **  @param bufferSize The maximal number of bytes to be read
   **  @param buffer The input buffer
   **  @param timeout A timeout in seconds after which the method must return.
   **         If 0.0, there will be no timeout.
   **  @return The number of bytes read
   **/
  int readBytes( const int bufferSize, char * buffer, const float timeout = 0.0 );

  /** Write exactly one byte to device. */
  void writeByte( char data );

  /** Write the given 0-terminated series of bytes to the device.*/
  void writeBytes( char *data );

  /** Write the given number of bytes to the device. */
  void writeBytes( char *data, uint len );

  /** Return the number of bytes waiting to be read. */
  int bytesAvailable();

protected:

  /** Establish a connection to the serial port.
   **  @throw SERIAL_INTERFACE_EXCEPTION_ON_OPEN
   **  @see SERIAL_INTERFACE_EXCEPTION_ON_OPEN
   **/
  void openDevice();

  /** Quit the existing connection. */
  void closeDevice();


protected:
  /** Get information about device. */
  struct termios * getConfigurationStatus();

  /** Descriptor structure for device. */
  struct termios m_configStatus;

  /** Is there an existing connection? */
  bool        m_bConnected;

  /** The device (e.g. /dev/ttyS0) */
  std::string m_sDeviceName;

  /** The file descriptor that is internally needed for handling
   ** the read/ write/ close operations */
  int         m_fileDescriptor;

};

#endif



