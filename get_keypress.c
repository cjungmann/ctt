#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>   // for exit()
#include <string.h>
#include <stdio.h>    // for dprintf() upon buffer overflow error

#include "ctt.h"

typedef void (*raw_mode_t)(void);

struct termios _termios_start;

void set_raw_mode_first(void);
void set_raw_mode_impl(void);

raw_mode_t set_raw_mode = set_raw_mode_first;

void set_raw_mode_first(void)
{
   int result = tcgetattr(STDIN_FILENO, &_termios_start);
   if (result)
   {
      const char *msg = "\ntcgetattr failed: aborting.\n";
      write(STDERR_FILENO, msg, strlen(msg));
      exit(1);
   }
   else
   {
      set_raw_mode = set_raw_mode_impl;
      (*set_raw_mode)();
   }
}

void set_raw_mode_impl(void)
{
   struct termios traw = _termios_start;

   // Unset some input mode flags
   traw.c_iflag &= ~( BRKINT | ICRNL | INPCK | ISTRIP | IXON );

   // Unset some output mode flags
   traw.c_oflag &= ~( OPOST );

   // Unset some control mode flags
   traw.c_cflag &= ~( CS8 );

   // Unset some local mode flags
   /* traw.c_lflag &= ~( ECHO | ICANON | IEXTEN | ISIG ); */
   traw.c_lflag &= ~( ECHO | ICANON | IEXTEN );

   tcsetattr(STDIN_FILENO, TCSAFLUSH, &traw);
}

void unset_raw_mode(void)
{
   tcsetattr(STDIN_FILENO, TCSAFLUSH, &_termios_start);
}

void set_read_mode(unsigned min_chars, unsigned timeout)
{
   struct termios tcur;
   tcgetattr(STDIN_FILENO, &tcur);
   tcur.c_cc[VMIN] = min_chars;
   tcur.c_cc[VTIME] = timeout;
   tcsetattr(STDIN_FILENO, TCSAFLUSH, &tcur);
}

void set_default_read_mode(void)
{
   struct termios tcur;
   tcgetattr(STDIN_FILENO, &tcur);
   tcur.c_cc[VMIN] = _termios_start.c_cc[VMIN];
   tcur.c_cc[VTIME] = _termios_start.c_cc[VTIME];
   tcsetattr(STDIN_FILENO, TCSAFLUSH, &tcur);
}

EXPORT int ctt_get_keypress(char *buff, int bufflen)
{
   int bytes_read;
   char *ptr = buff;

   // Set keypress detection conditions:
   (*set_raw_mode)();
   set_read_mode(1, 1); // Wait for at least 1 char, no longer than 1/10th second

   bytes_read = read(STDIN_FILENO, ptr, bufflen);

   if (bytes_read && bytes_read < bufflen)
   {
      ptr += bytes_read;
      *ptr = '\0';
   }
   else
      ptr = buff; // signal error

   // Restore normal conditions
   set_default_read_mode();
   unset_raw_mode();

   if (ptr == buff)
      dprintf(STDERR_FILENO, "\n\e[31;1mctt_get_keypress buffer overflow.\e[m\n");

   return ptr > buff;
}

#ifdef GET_KEYPRESS_MAIN

#include <stdio.h>
const char test_prefix[] = 
   "Press a key to see its output, 'q' to quit.\n"
   "Try function keys, use the control key, etc.\n"
   "\n"
   "Control characters will be red with a ^ prefix,\n"
   "\e[31m^[\e[m is the escape key press.\n"
   "\n";

void print_char_vals(const char *str)
{
   while (*str)
   {
      if (iscntrl(*str))
         printf("[31m^%c[m", (*str)+64);
      else
         printf("%c", *str);

      ++str;
   }
}

void test_buff_size(int buffsize)
{
   char *buff = (char*)alloca(buffsize);
   int count = 0;

   printf(test_prefix);

   while ( ctt_get_keypress(buff, buffsize) && *buff != 'q' )
   {
      printf("%-3d: ", ++count);
      print_char_vals(buff);
      printf("\n");
   }
}

int main(int argc, const char **argv)
{
   printf("Test with 10 character buffer.\n");
   test_buff_size(10);

   printf("\n\nTest with a 3 character buffer.  This should terminate prematurely.\n");
   test_buff_size(3);
}

#endif

/* Local Variables: */
/* compile-command: "b=get_keypress; \*/
/*  gcc -Wall -Werror -ggdb \*/
/*  -D${b^^}_MAIN           \*/
/*  -o $b ${b}.c            \*/
/*  -Wl,-R -Wl,. libctt.so"  */
/* End: */

