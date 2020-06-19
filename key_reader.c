#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>   // for exit()
#include <string.h>

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

   (*set_raw_mode)();

   // Wait for at least 1 character and then no more than 1/10th second for keypress
   set_read_mode(1, 1);
   // leave byte for terminator
   bytes_read = read(STDIN_FILENO, ptr, bufflen-1);
   ptr += bytes_read;
   *ptr = '\0';

   set_default_read_mode();
   
   unset_raw_mode();

   return 1;
}

#ifdef KEY_READER_MAIN

#include <stdio.h>

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

int main(int argc, const char **argv)
{
   char buff[10];
   int count = 0;

   printf("Press a key to see its output.\n");
   printf("\n");
   printf("This is more interesting with control characters.\n");
   printf("like an arrow key, a control key, etc.\n");
   printf("\n");
   printf("Control characters will be displayed in red with a ^ prefix.\n");
   printf("\n");
   printf("Press 'q' to quit.\n");
   printf("\n");

   while ( ctt_get_keypress(buff, sizeof(buff)) && *buff != 'q' )
   {
      printf("%-3d: ", ++count);
      print_char_vals(buff);
      printf("\n");
   }

   return 0;
}

#endif


/* Local Variables: */
/* compile-command: "b=key_reader; \*/
/*  gcc -Wall -Werror -ggdb \*/
/*  -D${b^^}_MAIN           \*/
/*  -o $b ${b}.c            \*/
/*  -Wl,-R -Wl,. libctt.so"  */
/* End: */

