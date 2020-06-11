// -*- compile-command: "cc -Wall -Werror -ggdb -o lss lss.c -lctt" -*-

#include <stdio.h>    // printf()
#include <stdlib.h>   // for malloc()

#include <errno.h>    // to use strerror()
#include <string.h>   // for strerror()

#include <alloca.h>

#include <ctt.h>

/*****************************************
 * CL Options, global variables and array
 ****************************************/
const char *basedir = ".";
int flagRecursive = 0;
int flagShowHelp = 0;
int BufferSize = 1024;

ctt_Option opts[] = {
   { 'h', "Show help",          &flagShowHelp,  NULL },
   { 'r', "Recursive search",   &flagRecursive, NULL },
   { 'd', "Base directory",     &basedir,       ctt_opt_set_string },
   { 'b', "Change buffer size", &BufferSize,    ctt_opt_set_int },
   OPTS_END
};

/* // Reference: man dircolors */
/* // Compare output of `dircolors` and `dircolors -p` */
/* // The list include more than the dirent d_type values. */

/* NString DType_Codes { */
/*    { DT_REG,     "rs" }, */
/*    { DT_DIR,     "di" }, */
/*    { DT_LNK,     "ln" }, */
/*    { DT_FIFO,    "pi" },   // pi_pe */
/*    { DT_SOCK,    "so" }, */
/*    { DT_BLK,     "bd" },  // b_lock d_evice */
/*    { DT_CHR,     "cd" },  // c_haracter d_evice */
/*    { DT_UNKNOWN, "mi" }, */
/*    END_STRNDX */
/* }; */

/* int count_lc_colors(char *colors) */
/* { */
/*    int count = 0; */
/*    while (*ptr) */
/*    { */
/*       if (*ptr == ':') */
/*          ++count; */

/*       ++ptr; */
/*    } */

/*    // We won't count a final unterminated color */
/*    // in case the supplied buffer is insufficient */
/*    // to contain the full LS_COLORS string. */
/* } */

/* void parse_lc_colors(char *buff, int bufflen) */
/* { */
/*    const char *e_ls_colors = getenv("LS_COLORS"); */
/*    int slen = strlen(e_ls_colors); */

/*    int copylen = bufflen - 1; */
/*    if (slen < bufflen) */
/*       copylen = slen; */

/*    memcpy(buff, e_ls_colors, copylen-1); */
/*    buff[copylen] = '\0'; */

/*    int color_count = count_lc_colors(buff); */
/*    int array_byte_length = color_count * sizeof(char*); */

/*    char **carray = (char**)alloca(array_byte_length); */
/*    memset(carray, 0, array_byte_length); */
/*    char **cptr = carray; */

/*    for (int i=0; i<color_count; ++i) */
/*    { */
/*    } */

/*    char *ptr = buff; */
/*    char *end, *name = buff; */
/*    while (*ptr) */
/*    { */
/*       while (*ptr && *ptr != '=') */
/*          ++ptr; */
/*       if (*ptr) */
/*       { */
         
/*       } */
/*    } */
   
/* } */

void scan_directory(const char *str, const char *indent);

void process_file(const char *name, const char *dir, char d_type, long inode, void *data)
{
   char *indent = (char *)data;

   if (strcmp(name,".") && strcmp(name,".."))
   {
      fputs(indent, stdout);

      if (d_type == DT_DIR)
         fputs("[32;1m", stdout);

      printf("%s/%s", dir, name);

      if (d_type == DT_DIR)
      {
         fputs("[m\n", stdout);

         int slen = strlen(dir) + strlen(name) + 2;  // + '/' + '\0'
         char *dirpath = (char*)alloca(slen);

         strcpy(dirpath, dir);
         strcat(dirpath, "/");
         strcat(dirpath, name);

         slen = strlen(indent);
         char *newindent = (char*)alloca(slen+3);
         strcpy(newindent, indent);
         strcat(newindent, "  ");
      
         scan_directory(dirpath, newindent);
      }
      else
         putchar('\n');
   }
   
}

void scan_directory(const char *str, const char *indent)
{
   char *buff = (char*)malloc(BufferSize);
   int eno;
   if ((eno = ctt_getdents(process_file, str, buff, BufferSize, (void*)indent)))
      fprintf(stderr, "'%s' not opened: %s.\n", str, strerror(eno));
   free(buff);
}

int main(int argc, const char **argv)
{
   ctt_process_options(opts, argc, argv);

   if (flagShowHelp)
   {
      ctt_show_options(opts);
      return 0;
   }

   scan_directory(basedir, "");

   return 0;
}
