#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "ctt.h"

#define EXPORT __attribute__((visibility("default")))

const ctt_Option *seek_option(const ctt_Option *options, char letter)
{
   const ctt_Option *ptr = options;
   while (ptr->letter)
   {
      if (ptr->letter == letter)
         return ptr;
      ++ptr;
   }

   return NULL;
}

EXPORT void ctt_show_options(const ctt_Option *options)
{
   const ctt_Option *ptr = options;
   while (ptr->letter)
   {
      printf("-%c: %s\n", ptr->letter, ptr->help);
      ++ptr;
   }
}

EXPORT void ctt_opt_set_int(const ctt_Option *option, const char *value)
{
   *((int*)option->target) = atoi(value);
}

EXPORT void ctt_opt_set_string(const ctt_Option *option, const char *value)
{
   *((const char **)option->target) = value;
}

EXPORT void ctt_process_options(const ctt_Option *options, int argc, const char **argv)
{
   // The initial value won't be used due to prefix increment in while loop
   const char **ptr = argv;
   
   const char **end = &argv[argc];
   const char *str;

   const ctt_Option *option;
   ctt_option_setter_t setfunc = NULL;

   while (++ptr < end)
   {
      str = *ptr;
      if (setfunc)
      {
         assert(option);
         (*setfunc)(option, str);
         setfunc = NULL;
         option = NULL;
      }
      else
      {
         while (*++str)
         {
            if ((option = seek_option(options, *str)))
            {
               if (option->setfunc)
               {
                  if (option->target)
                  {
                     setfunc = option->setfunc;
                     break;
                  }
                  else
                     (*option->setfunc)(option, NULL);
               }
               else
                  *(int*)option->target = 1;
            }
            else
            {
               printf("Unknown option %c.\n", *str);
            }
         }
      }
   }
}


#ifdef PROCESS_OPTIONS_MAIN

int main(int argc, const char **argv)
{
   return 0;
}

#endif


/* Local Variables: */
/* compile-command: "b=process_options; \*/
/*  gcc -Wall -Werror -ggdb \*/
/*  -D${b^^}_MAIN           \*/
/*  -o $b ${b}.c            \*/
/*  -Wl,-R -Wl,. libctt.so"  */
/* End: */

