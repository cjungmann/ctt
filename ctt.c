#include <termios.h>
#include <unistd.h>
#include <ctype.h>

#include <sys/types.h>  // for open()
#include <sys/stat.h>   //   "
#include <fcntl.h>      //   "

#include "ctt.h"

struct termios _termios_start = {0};
int ctt_started = 0;
int tty_handle = -1;

EXPORT void ctt_start(void)
{
   if (!ctt_started)
   {
      tcgetattr(STDIN_FILENO, &_termios_start);
      tty_handle = open("/dev/tty", O_RDWR);
      ctt_started = 1;
   }
}

EXPORT int ctt_is_started(void)
{
   return ctt_started && tty_handle > -1;
}
