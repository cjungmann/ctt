#include <stdio.h>   // for printf, etc
#include <ctype.h>   // for iscntrl()
#include <stdlib.h>  // for malloc()

/* #include <sys/types.h> // */
/* #include <sys/stat.h>  // */
#include <fcntl.h>     //  for open()
#include <unistd.h>    /// for close() !?


#include "ctt.h"

int flagShowHelp = 0;
int flagKeyReader = 0;
int flagLineReader = 0;
const char *FileName = NULL;
unsigned int BufferSize = 0;

ctt_Option opts[] = {
   { 'h', "Show this help screen.", &flagShowHelp,   NULL },
   { 'f', "Set the filename.",      &FileName,       ctt_opt_set_string },
   { 'b', "Set the buffer size.",   &BufferSize,     ctt_opt_set_int },
   { 'r', "Run key_reader demo.",   &flagKeyReader,  NULL },
   { 'l', "Run line_reader demo.",  &flagLineReader, NULL },
   OPTS_END
};

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

const char *line_reader_demo_text =
   "\n"
   "You are about to the run the line_reader demonstration.\n"
   "\n"
   "If you entered a buffer size and file name, these values\n"
   "will be used for the demo.  Otherwise, default values will\n"
   "be used.\n"
   "\n";


void demonstrate_line_reader(void)
{
   int file_handle = -1;

   puts(line_reader_demo_text);

   if (BufferSize)
      printf("Using command-line defined buffer size %u.\n", BufferSize);
   else
   {
      BufferSize = 2048;
      printf("Using buffer size %u in lieu of missing command-line setting.\n", BufferSize);
   }

   if (!FileName)
   {
      FileName = "test.c";
      printf("Using FileName '%s' in lieu of missing command-line setting.\n", FileName);
   }

   file_handle = open(FileName, O_RDONLY);
   if (file_handle > -1)
   {
      char *buffer = (char*)malloc(BufferSize);
      LRScope scope;
      ctt_init_line_reader(&scope, buffer, BufferSize, file_handle);

      const char *line, *line_end;
      int count = 0;
      while( ctt_get_line(&scope, &line, &line_end))
         printf("[32m%-3d :[m %s\n", ++count, line);

      close(file_handle);
   }
}

const char *keyreader_demo_text = "\n"
   "You are about to test the key_reader feature.\n"
   "\n"
   "Press any key and see the characters that it represents.\n"
   "Most keys are not very interesting, but control keys, arrow\n"
   "keys, etc, are represented by multiple characters.\n"
   "Experiment.\n"
   "\n"
   "Type 'q' to quit this test section.\n"
   "\n";
   

void demonstrate_key_reader(void)
{
   char buff[10];
   ctt_init_key_reader();

   puts(keyreader_demo_text);

   while (ctt_get_keypress(buff, sizeof(buff)) && *buff != 'q')
   {
      print_char_vals(buff);
      printf("\n");
   }

   printf("...you pressed 'q' to finish this demonstration.\n");
}

int main(int argc, const char **argv)
{
   ctt_process_options(opts, argc, argv);

   if (flagShowHelp)
      ctt_show_options(opts);
   else
   {
      if (FileName)
         printf("You set filename to %s.\n", FileName);
      else
         printf("You did not set the filename.\n");

      if (BufferSize)
         printf("You set the buffer size to %d.\n", BufferSize);
      else
         printf("You did not set the buffer size.\n");

      if (flagKeyReader)
         demonstrate_key_reader();

      if (flagLineReader)
         demonstrate_line_reader();
      
   }

   return 0;
}
