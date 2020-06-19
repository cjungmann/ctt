#ifndef CTT_H
#define CTT_H

#include <termios.h>
#include <dirent.h>

#define EXPORT __attribute__((visibility("default")))

#include "cusses.h"


/************************
 * ctt_indexed_string()
 ***********************/                      
typedef struct _indexed_string
{
   int index;
   const char *str;
} NString;

#define END_STRNDX { -1, NULL }
const char *ctt_indexed_string(const NString *nstrings, int value);



/* Generic structure for identifying placement intentions. */
typedef struct ctt_object_placement
{
   int   row;
   int   column;
   int   height;
   int   width;
} ObjPlace;

/*******************
 * ctt_dir_walk_*()
 ******************/

typedef struct linux_dirent LDirent;

typedef struct ctt_dir_walk_env {
   const char *dir;
   char       *buffer;
   int        buffer_len;
   LDirent    *data_ptr;
   LDirent    *end_ptr;

   int        fd;
   int        error_state;
} DWalkEnv;

int ctt_dir_walk_init(DWalkEnv *env, const char *path, char *buffer, int buffer_len);
int ctt_dir_walk(DWalkEnv *env, const char **name, char *type, long *inode);
void ctt_dir_walk_release(DWalkEnv *env);

/******************************************
 * ctt_getdents()
 * Deprecated: use ctt_dir_walk*() instead
 *****************************************/
// Calback function for ctt_getdents()
typedef void (*ctt_usedent_t)(const char *name,
                              const char *dir,
                              char d_type,
                              long inode,
                              void *data);

int ctt_getdents(ctt_usedent_t duser,
                 const char *dirpath,
                 char *buff,
                 int bufflen,
                 void *data);


/************************************
 * ctt_get_line()
 * Line Reader struct and functions 
 ************************************/
typedef struct ctt_line_reader
{
   char *buffer;
   char *buffer_end;

   char *data_end;
   char *line_ptr;

   int  eof;

   int  file_handle;
} LRScope;

int ctt_init_line_reader(LRScope *scope, char *buffer, int buffsize, int file_handle);
int ctt_get_line(LRScope *scope, const char** line, const char** line_end);


/************************
 * Key Reader functions
 ***********************/

int ctt_get_keypress(char *buff, int bufflen);

/***********************
 * list_pick.c function
 **********************/

int ctt_pick_from_list(ObjPlace *placement,
                       const char **list,
                       int list_count,
                       int top,
                       int selected);

/**********************************
 * Command-line Options processing
 *********************************/
struct _ctt_option;

typedef void (*ctt_option_setter_t)(const struct _ctt_option*, const char*);

typedef struct _ctt_option
{
   char                letter;  // Letter, following a dash '-', used to set option
   const char          *help;   // string describing the option
   void                *target; // Fungible pointer on which setfunc may work

   ctt_option_setter_t setfunc; // optional function pointer to process the option value
} ctt_Option;

#define OPTS_END {'\0',NULL,NULL,NULL}

void ctt_opt_set_int(const ctt_Option *option, const char *value);
void ctt_opt_set_string(const ctt_Option *option, const char *value);

void ctt_show_options(const ctt_Option *options);
void ctt_process_options(const ctt_Option *options, int argc, const char **argv);

/**
 * Miscellaneous functions
 */

int ctt_itoa_buff(int value, int base, char *buffer, int buffer_len);


#endif
