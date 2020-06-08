#ifndef CTT_H
#define CTT_H

#define EXPORT __attribute__((visibility("default")))
extern struct termios _termios_start;
extern int tty_handle;

/*************************
 * Base Library functions
 ************************/

void ctt_start(void);
int ctt_is_started(void);

/************************************
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


#endif
