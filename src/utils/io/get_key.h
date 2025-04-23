#ifndef UTILS_GET_KEY
#define UTILS_GET_KEY

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>

/** Read a value from keyboard. If blocking is 1, this function will block
 *  until a key is pressed.
 */
char getKey( int blocking = 0 );

#endif
