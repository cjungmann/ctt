#include <string.h>
#include "ctt.h"

EXPORT int ctt_itoa_buff(int value, int base, char *buffer, int buffer_len)
{
   memset(buffer, 0, buffer_len);
   char *end = buffer + buffer_len -1; // Leave room for \0
   char *ptr = buffer;

   while (ptr < end && value > 0)
   {
      *ptr = (value % base) + '0';
      ++ptr;
      value /= base;
   }

   if (value)
      return 0;   // overflow
   else
   {
      // Set reverse boundaries
      --ptr;
      end = buffer;

      int save;
      while (ptr > end)
      {
         save = *end;
         *end = *ptr;
         *ptr = save;

         ++end;
         --ptr;
      }
      return 1;
   }
}

