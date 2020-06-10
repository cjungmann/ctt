// -*- compile-command: "cc -Wall -Werror -ggdb -DDIR_ITERATE_MAIN -o dir_iterate dir_iterate.c" -*-

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>    // for strerror()
#include <stdio.h>

#include <fcntl.h>
#include <unistd.h>    // for close()

#include <stdint.h>
#include <sys/syscall.h>  // for syscall()
#include <alloca.h>

#include "ctt.h"

// Private structure to interpret getdents:
typedef struct linux_dirent {
   long     d_inode;
   off_t    d_offset;
   uint16_t d_reclen;
   char     d_name[];
} LDirent;

EXPORT int ctt_getdents(ctt_usedent_t duser, const char *dirpath, char *buff, int bufflen, void *data)
{
   int fd = open(dirpath, O_RDONLY | O_NONBLOCK | O_DIRECTORY);
   int bytes_read, buff_index;
   int d_type;
   LDirent *dptr;
   if (fd)
   {
      while (1)
      {
         bytes_read = syscall(SYS_getdents, fd, buff, bufflen);

         if (bytes_read == -1)
            break;

         if (bytes_read == 0)
            break;

         buff_index = 0;
         while (buff_index < bytes_read)
         {
            dptr = (LDirent*)(buff + buff_index);
            d_type = *(char*)((char*)dptr + dptr->d_reclen - 1);

            (*duser)(dptr->d_name, dirpath, d_type, dptr->d_inode, data);

            buff_index += dptr->d_reclen;
         }
      }

      close(fd);
   }

   return errno;
}

#ifdef DIR_ITERATE_MAIN

// include needed library source files
#include "ctt.c"
#include "procopts.c"

// Command-line options data
int show_Help = 0;
int getdents_BuffSize = 1024;
int use_getdents = 0;
int getdents_colored_type = 0;
const char *dirpath = ".";

ctt_Option opts[] = {
   { 'h', "See options", &show_Help, NULL },
   { 'd', "Set directory path", &dirpath, ctt_opt_set_string }, 
   { 'b', "Set getdents buffer size", &getdents_BuffSize, ctt_opt_set_int },
   { 'g', "Use getdents for stack-based", &use_getdents, NULL },
   { 'c', "Colorize files by type", &getdents_colored_type, NULL },
   OPTS_END
};

NString DType_Names[] = {
   { DT_REG,     "regular file" },
   { DT_DIR,     "directory" },
   { DT_LNK,     "symbolic link" },
   { DT_FIFO,    "named FIFO pipe" },
   { DT_SOCK,    "socket" },
   { DT_BLK,     "block device" },
   { DT_CHR,     "character device" },
   { DT_UNKNOWN, "unknown" },
   END_STRNDX
};

// You can test links, block and character devices in /dev directory
NString DType_Colors[] = {
   { DT_REG,     "" },
   { DT_DIR,     "[34;1m" },
   { DT_LNK,     "[36;1m" },
   { DT_FIFO,    "[33;1;48;5;236m" },  // using VT100 background (48;5;nnn) colors
   { DT_SOCK,    "[32;1;48;5;236m" },  // using VT100 background (48;5;nnn) colors
   { DT_BLK,     "[33;2m" },
   { DT_CHR,     "[33;1m" },
   { DT_UNKNOWN, "[31;1m" },
   END_STRNDX
};

void dent_user_colored(const char *name, const char *dir, char type, long inode, void *data)
{
   int *count = (int*)data;

   const char *colstr = get_indexed_string(DType_Colors, type);
   if (!colstr)
      colstr = "[31;1m";

   printf("%-5d: %s%s[m (%ld)\n", ++(*count), colstr, name, inode);
}

void dent_user_named(const char *name, const char *dir, char type, long inode, void *data)
{
   int *count = (int*)data;

   const char *colname = get_indexed_string(DType_Names, type);
   if (!colname)
      colname = "un-typed";

   printf("%-5d: %s (%s : %ld)\n", ++(*count), name, colname, inode);
}

ctt_usedent_t dent_user = dent_user_named;

void with_getdents(void)
{
   char *buff = (char*)alloca(getdents_BuffSize);
   int count = 0;
   ctt_getdents(dent_user, dirpath, buff, getdents_BuffSize, &count);
}

// As control, for comparison with getdents() version
void with_opendir(void)
{
   DIR *fstr = opendir(dirpath);
   if (fstr)
   {
      struct dirent *dire;
      int count = 0;

      while ((dire = readdir(fstr)))
      {
         const char *typename = get_indexed_string(DType_Names, dire->d_type);
         if (!typename)
            typename = "un-typed";
         printf("%-4d: %s (%s)\n", ++count, dire->d_name, typename);
      }

      closedir(fstr);
   }
   else
   {
      printf("Failed to open %s failed with error %s (%d).\n",
             dirpath,
             strerror(errno),
             errno);
   }
}

int main(int argc, const char **argv)
{
   ctt_process_options(opts, argc, argv);

   if (show_Help)
   {
      ctt_show_options(opts);
      return 0;
   }

   if (getdents_colored_type)
      dent_user = dent_user_colored;

   if (use_getdents)
      with_getdents();
   else
      with_opendir();

   return 0;
}

#endif
