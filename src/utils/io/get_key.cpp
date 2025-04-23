#include <utils/io/get_key.h>

//-----------------------------------------------------------------------------
void set_nonblock_flag( void )
//-----------------------------------------------------------------------------
{
  int oldflags;

  oldflags  = fcntl( STDIN_FILENO, F_GETFL, 0 );
  oldflags |= O_NONBLOCK;
  fcntl( STDIN_FILENO, F_SETFL, oldflags );
}


//-----------------------------------------------------------------------------
void clear_nonblock_flag( void )
//-----------------------------------------------------------------------------
{
  int oldflags;

  oldflags  = fcntl( STDIN_FILENO, F_GETFL, 0 );
  oldflags &= ~O_NONBLOCK;
  fcntl( STDIN_FILENO, F_SETFL, oldflags );
}


//-----------------------------------------------------------------------------
char getKey( int blocking )
//-----------------------------------------------------------------------------
{
  char buf[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  ssize_t n = 0;
  struct termios tattr,              // new terminal attributes
  saved_attributes;              // restore the original settings of the terminal

  set_nonblock_flag();
  tcgetattr( STDIN_FILENO, &saved_attributes );   // save the original attributes

  tcgetattr( STDIN_FILENO, &tattr );              // set the new attributes for the terminal:
  tattr.c_lflag   &= ~(ICANON);		    // Clear ICANON
  tattr.c_lflag   &= ~(ECHO);		            // and ECHO
  tattr.c_cc[VMIN] = 0;                           // noncanonical, direction transmission
  tattr.c_cc[VTIME]= 0;                           // of input characters (MIN=0,TIME=0)
  tcsetattr( STDIN_FILENO, TCSANOW, &tattr );

  if ( blocking == 1 )
  {
    while( n == 0 )
    {
      n = read( STDIN_FILENO, buf, 1 );
    }
  }
  else
  {
    n = read( STDIN_FILENO, buf, 1 );
  }

  tcsetattr( STDIN_FILENO, TCSANOW, &saved_attributes );
  clear_nonblock_flag();

  return buf[0];
}
