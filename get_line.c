// -*- compile-command: "cc -Wall -Werror -ggdb -DGET_LINE_MAIN -o get_line get_line.c -lclargs" -*-

#include <stdio.h>   // define NULL and fprintf()

#include <fcntl.h>    // for open()
#include <unistd.h>   // for read()
#include <string.h>   // memset()

#include <errno.h>    // to access errno information

#include "ctt.h"

#define EXPORT __attribute__((visibility("default")))

int _buffer_needs_refresh(const LRScope *scope)
{
   return scope->line_ptr >= scope->data_end;
}

int _buffer_size(const LRScope *scope)
{
   return scope->buffer_end - scope->buffer;
}

/**
 * Walks chars to find a nl or cr + nl, changing
 * these characters to '\0' if found, and returning
 * the first character that signalled the end of
 * the string.
 */
char* _seek_line_end(char *ptr, char *limit)
{
   char *rval = NULL;
   while (ptr < limit)
   {
      if (*ptr == '\n')
      {
         rval = ptr;
         *ptr = '\0';
         break;
      }
      else if (*ptr == '\r' && *(ptr+1) == '\n')
      {
         rval = ptr;
         *ptr = '\0';
         *++ptr = '\0';
         break;
      }

      ++ptr;
   }

   return rval;
}

/**
 * Move last line fragment to beginning of buffer, then
 * update the data_end to the end of the fragment to prepare
 * for a subsequent read to continue the line.
 *
 * Updates LRScope members line_ptr and data_end.
 * 
 * We assume _shift_buffer() is called because we don't have
 * the line_end, setting that member is left to a later call
 * to _seek_line_end();
 */
void _shift_buffer(LRScope *scope)
{
   int bytes_to_move = scope->data_end - scope->line_ptr;
   memmove(scope->buffer, scope->line_ptr, bytes_to_move);
   scope->line_ptr = scope->buffer;
   scope->data_end = scope->buffer + bytes_to_move;
}

/**
 * Fills the buffer from data_end to buffer_end.
 *
 * Updates LRScope members data_end, line_ptr, and line_end.
 */
int _refresh_buffer(LRScope *scope)
{
   size_t bytes_to_read = scope->buffer_end - scope->data_end;
   ssize_t bytes_read;

   bytes_read = (*scope->reader)(scope->source, scope->data_end, bytes_to_read);
   if (bytes_read > 0)
   {
      // Set data boundaries
      scope->data_end += bytes_read;

      // Overwrite previous data_end char if data_end before buffer_end
      if (scope->data_end < scope->buffer_end)
         *scope->data_end = '\0';

      scope->line_ptr = scope->buffer;

      return 1;
   }
   else
   {
      scope->line_ptr = scope->data_end = scope->buffer;
      scope->eof = 1;
      return 0;
   }
}


EXPORT int ctt_init_line_reader(LRScope *scope,
                                char *buffer,
                                int buffsize,
                                ctt_read_source rfunc,
                                void *source)
{
   memset(scope, 0, sizeof(LRScope));

   scope->buffer = buffer;
   scope->buffer_end = scope->buffer + buffsize;
   scope->data_end = scope->line_ptr = scope->buffer;

   scope->reader = rfunc;
   scope->source = source;

   return _refresh_buffer(scope);
}

EXPORT void ctt_reset_line_reader(LRScope *scope)
{
   scope->data_end = scope->line_ptr = scope->buffer;
   scope->eof = 0;
}


int read_source_from_file(void *source, char *buffer, int bytes_to_read)
{
   int fh = *(int*)source;
   return read(fh, buffer, bytes_to_read);
}

EXPORT int ctt_init_line_reader_with_file(LRScope *scope,
                                          char *buffer,
                                          int buffsize,
                                          int *file_handle)
{
   return ctt_init_line_reader(scope, buffer, buffsize,
                               read_source_from_file,
                               file_handle);
}

EXPORT int ctt_get_line(LRScope *scope, const char** line, const char** line_end)
{
   char *end = NULL;
   if (scope->line_ptr)
   {
      end = _seek_line_end(scope->line_ptr, scope->data_end);
      if (!end)
      {
         _shift_buffer(scope);
         _refresh_buffer(scope);

         if (scope->eof)
            return 0;
         else
            end = _seek_line_end(scope->line_ptr, scope->data_end);
      }
   }

   if (end)
   {
      *line = scope->line_ptr;
      *line_end = end;

      while (end < scope->data_end && *end == '\0')
         ++end;

      scope->line_ptr = end;

      return 1;
   }
   else if (scope->buffer != scope->data_end)
      fprintf(stderr, "Buffer size %d insufficient for line in file.\n", _buffer_size(scope));

   return 0;
}


#ifdef GET_LINE_MAIN

#include <stdio.h>
#include <clargs.h>
#include <alloca.h>

const char *filepath = NULL;
int buffsize = 16384;

void read_file(int file_handle)
{
   LRScope scope;
   char *buffer = (char*)alloca(buffsize);
   ctt_init_line_reader(&scope, buffer, buffsize, file_handle);

   const char *line, *line_end;

   while(ctt_get_line(&scope, &line, &line_end))
   {
      printf("%s\n", line);
   }
}

void show_help(const DefLine *option, const char *value);

DefLine opts[] = {
   { 'h', "Show help.",       NULL,      show_help },
   { 'b', "Set buffer size.", &buffsize, clargs_set_int },
   { 'f', "File to read.",    &filepath, clargs_set_string },
   CLARGS_END_DEF
};

void show_help(const DefLine *option, const char *value)
{
   clargs_show(opts);
}

int main(int argc, const char **argv)
{
   int file_handle;
   clargs_process(opts, argc, argv);

   if (filepath)
   {
      file_handle = open(filepath, O_RDONLY);
      if (file_handle != -1)
      {
         read_file(file_handle);
         close(file_handle);
         return 0;
      }
      else
         return 1;
   }
   else
   {
      clargs_show(opts);
      return 1;
   }

}

#endif

/* Local Variables: */
/* compile-command: "b=get_line; \*/
/*  gcc -Wall -Werror -ggdb \*/
/*  -D${b^^}_MAIN           \*/
/*  -o $b ${b}.c            \*/
/*  -lclargs"                */
/* End: */
