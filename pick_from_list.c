#include <stdio.h>
#include <string.h>

#include "ctt.h"

void show_list(int row, int column,
               int rows, int columns,
               int top,
               int selected,
               const char **texts,
               const char **end)
{
   ctt_set_cursor(row,0);

   const char **ptr = texts + top;
   const char **bottom = ptr + rows;
   const char **sel = texts + selected;
   while (ptr < end && ptr < bottom)
   {
      ctt_cursor_right(column);

      if (ptr == sel)
         printf("[43m");

      printf("%-*s", columns, *ptr);

      if (ptr == sel)
         printf("[m");

      putchar('\n');
      ++ptr;
   }
}

int is_uparrow(const char *buff) { return 0 == strcmp(buff, "[A"); }
int is_downarrow(const char *buff) { return 0 == strcmp(buff, "[B"); }
int is_enter(const char *buff) { return *buff == 13; }

EXPORT int ctt_pick_from_list(ObjPlace *placement,
                              const char **list,
                              int list_count,
                              int top,
                              int selected)
{
   char kpbuff[10];

   const char **end = list + list_count;

   while (1)
   {
      show_list(placement->row,
                placement->column,
                placement->height,
                placement->width,
                top, selected,
                list, end);

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
         if (selected < list_count-1)
            ++selected;
         if (selected > top + placement->height - 1)
            ++top;
      }
      else if (is_enter(kpbuff))
         return selected;
      else if (*kpbuff == 'q')
         break;
   }

   return -1;
}


#ifdef PICK_FROM_LIST_MAIN

#include <stdio.h>
#include <unistd.h>

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

const char** seek_end(const char **list)
{
   while (*list)
      ++list;
   return list;
}

void fill_screen(int fill_char)
{
   int rows, cols;

   ctt_clear();
   ctt_get_screen_size(&rows, &cols);

   for (int r=0; r<rows; ++r)
   {
      for (int c=0; c<cols; ++c)
         write(STDIN_FILENO, &fill_char, 1);
      write(STDIN_FILENO, "\n", 1);
   }
}

int main(int argc, const char **argv)
{
   ObjPlace placement = { 10, 10, 10, 40 };

   fill_screen('.');

   int selected = ctt_pick_from_list(&placement,
                                     items,
                                     seek_end(items) - items,
                                     0, 0);

   if (selected > -1)
      printf("You selected item %d (%s).\n", selected, items[selected]);

   return 0;
}

#endif

/* Local Variables: */
/* compile-command: "b=pick_from_list; \*/
/*  gcc -Wall -Werror -ggdb \*/
/*  -D${b^^}_MAIN           \*/
/*  -o $b ${b}.c            \*/
/*  -Wl,-R -Wl,. libctt.so"  */
/* End: */
