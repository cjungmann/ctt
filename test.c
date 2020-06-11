#include <stdio.h>   // for printf, etc
#include <ctype.h>   // for iscntrl()
#include <stdlib.h>  // for malloc()

/* #include <sys/types.h> // */
/* #include <sys/stat.h>  // */
#include <fcntl.h>     //  for open()
#include <unistd.h>    /// for close() !?

#include <dirent.h>    // for DT_ file types
#include <string.h>    // for strerror()

#include "ctt.h"

int flagShowHelp = 0;
int flagKeyReader = 0;
int flagLineReader = 0;
int flagListPick = 0;
const char *FileName = NULL;
const char *GetDentsDir = NULL;
unsigned int BufferSize = 1024;


ctt_Option opts[] = {
   { 'h', "Show this help screen.", &flagShowHelp,   NULL },
   { 'f', "Set the filename.",      &FileName,       ctt_opt_set_string },
   { 'b', "Set the buffer size.",   &BufferSize,     ctt_opt_set_int },
   { 'r', "Run key_reader demo.",   &flagKeyReader,  NULL },
   { 'l', "Run line_reader demo.",  &flagLineReader, NULL },
   { 'p', "Run list_pick demo.",    &flagListPick,   NULL },
   { 'd', "Set getdents dir.",      &GetDentsDir,    ctt_opt_set_string },
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
   ctt_start();

   puts(keyreader_demo_text);

   while (ctt_get_keypress(buff, sizeof(buff)) && *buff != 'q')
   {
      print_char_vals(buff);
      printf("\n");
   }

   printf("...you pressed 'q' to finish this demonstration.\n");
}


void fill_screen(int fill_char)
{
   int rows, cols;

   ctt_clear();
   ctt_get_screen_size(&rows, &cols);

   for (int r=0; r<rows; ++r)
   {
      for (int c=0; c<cols; ++c)
         write(tty_handle, &fill_char, 1);
      write(tty_handle, "\n", 1);
   }
}

void demonstrate_list_pick(void)
{
   const char *items[] = {
      "Mom",
      "Dad",
      "Sister",
      "Brother",
      "Aunt",
      "Uncle",
      "Grandma",
      "Grandpa",
      "Cousin",
      "Son",
      "Daughter",
      "Son-in-law",
      "Daughter-in-law",
      "Granddaughter",
      "Grandson"
   };

   int array_len = sizeof(items) / sizeof(char*);

   ObjPlace placement = { 10, 10, 10, 40 };

   fill_screen('.');

   int selected = ctt_pick_from_list(&placement,
                                     items,
                                     array_len,
                                     0, 0);

   if (selected > -1)
      printf("You selected item %d (%s).\n", selected, items[selected]);

   ctt_clear();
}

/**************************************************************
 * demonstrate_getdents() support array, callback and function
 *************************************************************/

// You can test links, block and character devices in /dev directory
NString DType_Colors[] = {
   { DT_REG,     "" },
   { DT_DIR,     "\e[34;1m" },
   { DT_LNK,     "\e[36;1m" },
   { DT_FIFO,    "\e[33;1;48;5;236m" },  // using VT100 background (48;5;nnn) colors
   { DT_SOCK,    "\e[32;1;48;5;236m" },  // using VT100 background (48;5;nnn) colors
   { DT_BLK,     "\e[33;2m" },
   { DT_CHR,     "\e[33;1m" },
   { DT_UNKNOWN, "\e[31;1m" },
   END_STRNDX
};

void usedent_callback(const char *name,
                      const char *dir,
                      char d_type,
                      long inode,
                      void *data)
{
   int *count = (int*)data;
   const char *tcolor = ctt_indexed_string(DType_Colors, d_type);

   printf("%-4d: %s%s[m\n", ++(*count), tcolor, name);
}

void demonstrate_getdents(void)
{
   int count = 0;
   char *buff = (char*)alloca(BufferSize);
   if (buff)
   {
      int rerrno = ctt_getdents(usedent_callback, GetDentsDir, buff, BufferSize, &count);
      if (rerrno)
         printf("ctt_getdents failed with %s.\n", strerror(rerrno));
   }
   else
      printf("Failed to secure %d memory.\n", BufferSize);
}



int main(int argc, const char **argv)
{
   ctt_start();

   ctt_process_options(opts, argc, argv);

   printf("GetDentsDir is \"%s\"\n", GetDentsDir);

   if (flagShowHelp)
      ctt_show_options(opts);
   else if (GetDentsDir)
      demonstrate_getdents();
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

      if (flagListPick)
         demonstrate_list_pick();
      
   }

   return 0;
}
