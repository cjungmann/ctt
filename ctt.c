#include <termios.h>
#include <unistd.h>
#include <ctype.h>

#include <sys/types.h>  // for open()
#include <sys/stat.h>   //   "
#include <fcntl.h>      //   "

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "ctt.h"

EXPORT const char *ctt_indexed_string(const NString *nstrings, int value)
{
   while (nstrings)
   {
      if (nstrings->index == value)
         return nstrings->str;
      else if (nstrings->index == -1)
         break;

      ++nstrings;
   }

   return NULL;
}

