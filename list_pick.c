#include "ctt.h"

#include <stdio.h>
#include <string.h>

void show_list(int row, int column,
               int rows, int columns,
               int top,
               int selected,
               const char **texts,
               const char **end)
{
   ctt_clear();
   ctt_set_cursor(row,0);

   const char **ptr = texts + top;
   const char **bottom = ptr + rows;
   const char **sel = texts + selected;
   while (ptr < end && ptr < bottom)
   {
      ctt_cursor_right(column);

      if (ptr == sel)
         printf("[43m");

      printf("%s", *ptr);

      if (ptr == sel)
         printf("[m");

      putchar('\n');
      ++ptr;
   }
}

const char** seek_end(const char **list)
{
   while (*list)
      ++list;
   return list;
}

int is_uparrow(const char *buff) { return 0 == strcmp(buff, "[A"); }
int is_downarrow(const char *buff) { return 0 == strcmp(buff, "[B"); }
int is_enter(const char *buff) { return *buff == 13; }

EXPORT int ctt_pick_from_list(const char **list,
                              const char **end,
                              int rows_allowed,
                              int cols_allowed,
                              int top,
                              int selected)
{
   char kpbuff[10];

   int count = end - list;

   while (1)
   {
      show_list(10, 10, 10, 80,
                top, selected,
                list, end);

      printf("\nThere are %d items in the list.\n", count);

      ctt_get_keypress(kpbuff, sizeof(kpbuff));

      if (is_uparrow(kpbuff))
      {
         if (selected > 0)
         {
            --selected;
            if (selected < top)
               --top;
         }
      }
      else if (is_downarrow(kpbuff))
      {
         if (selected < count-1)
            ++selected;
         if (selected > top + rows_allowed - 1)
            ++top;
      }
      else if (is_enter(kpbuff))
         return selected;
      else if (*kpbuff == 'q')
         break;
   }

   return -1;
}


#ifdef LISTPICK_MAIN

#include <stdio.h>

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
   "Grandson",
   NULL
};


int main(int argc, const char **argv)
{
   ctt_start();

   int selected = ctt_pick_from_list(items,
                                     seek_end(items),
                                     10, 80,
                                     0, 0);

   if (selected > -1)
      printf("You selected item %d (%s).\n", selected, items[selected]);

   /* const char **end = seek_end(items); */
   /* char kpbuff[10]; */

   /* int top = 0; */
   /* int selected = 0; */
   /* int count = end - items; */

   /* while (1) */
   /* { */
   /*    show_list(0, 0, 10, 80, */
   /*              top, selected, */
   /*              items, end); */

   /*    printf("\nThere are %d items in the list.\n", count); */

   /*    ctt_get_keypress(kpbuff, sizeof(kpbuff)); */

   /*    if (is_uparrow(kpbuff)) */
   /*    { */
   /*       if (selected > 0) */
   /*          --selected; */
   /*    } */
   /*    else if (is_downarrow(kpbuff)) */
   /*    { */
   /*       if (selected < count-1) */
   /*          ++selected; */
   /*    } */
   /*    else if (is_enter(kpbuff)) */
   /*    { */
   /*       printf("You selected %d (%s).\n", selected, items[selected]); */
   /*       break; */
   /*    } */
   /*    else if (*kpbuff == 'q') */
   /*       break; */
   /* } */

   return 0;
}

#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcomment"
// Local Variables:
// compile-command : "cc -Wall -Werror -ggdb \
//    -DLISTPICK_MAIN \
//    -o list_pick list_pick.c \
//    -Wl,-R -Wl,. libctt.so"
// End:
#pragma GCC diagnostic pop

