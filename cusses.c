#include <sys/ioctl.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>  // memset()
#include <errno.h>
#include <stdlib.h>  // for atoi()

#include "ctt.h"


// Private functions in key_reader.c
void set_raw_mode(void);
void unset_raw_mode(void);

int tty_write(const char *str)           { return write(tty_handle, str, strlen(str)); }
int tty_read(char *buffer, int len_buff) { return read(tty_handle, buffer, len_buff); }

EXPORT void ctt_cursor_right(int chars)      { dprintf(tty_handle, "[%dC", chars); }

EXPORT void ctt_clear(void)                  { tty_write("[2J"); }
EXPORT void ctt_set_cursor(int row, int col) { dprintf(tty_handle, "[%d;%dH", row, col); }

EXPORT void ctt_get_cursor(int *row, int *col)
{
   char buff[20];

   *row = *col = -1;

   set_raw_mode();
 
   int bytes_xfer = tty_write("[6n");
   bytes_xfer = tty_read(buff, sizeof(buff));

   unset_raw_mode();

   if (bytes_xfer < 1)
      printf("Failure getting cursor position (%d) %s.\n", errno, strerror(errno));
   else
   {
      if (strncmp(buff, "[", 2)==0)
      {
         char *ptr = buff + 2;
         char *end = ptr;

         while (*end)
         {
            if (*end == ';')
            {
               // save the number
               *end = '\0';
               *row = atoi(ptr);

               // prepare for next number:
               ptr = ++end;
            }
            else if (*end == 'R')
            {
               *end = '\0';
               *col = atoi(ptr);
               break;
            }
            else
               ++end;
         }
      }
   }
}

EXPORT void ctt_get_screen_size(int *rows, int *cols)
{
   // Refer to man iocto_tty(2)
   struct winsize size;
   int rval = ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
   if (!rval)
   {
      *rows = size.ws_row;
      *cols = size.ws_col;
   }
}

#ifdef CCONTROL_MAIN

#include "ctt.c"
#include "key_reader.c"

int main(int argc, const char **argv)
{
   ctt_start();

   int rows, cols;
   printf("In console_control.\n");

   ctt_get_screen_size(&rows, &cols);
   printf("The screen has %d rows of %d columns.\n", rows, cols);

   ctt_get_cursor(&rows, &cols);
   printf("The cursor is at row %d, column %d.\n", rows, cols);

   printf("Done with the main function.\n");
   return 0;
}

#endif




#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcomment"
// Local Variables:
// compile-command : "cc -Wall -Werror -ggdb \
//    -DCCONTROL_MAIN \
//    -o cusses cusses.c \
//    -Wl,-R -Wl,. libctt.so"
// End:
#pragma GCC diagnostic pop

