#include <dirent.h>
#include <stdint.h>       // for uint16_t
#include <sys/syscall.h>  // for syscall()

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>       // for close()
#include <string.h>       // for memset()

#include "ctt.h"

// Private structure to interpret getdents:
typedef struct linux_dirent {
   long     d_inode;
   off_t    d_offset;
   uint16_t d_reclen;
   char     d_name[];
} LDirent;

int at_buffer_end(DWalkEnv *env) { return env->data_ptr >= env->end_ptr; }

/** Accept parameter as void* to avoid struct alignment issues. */
LDirent *dirent_increment(void *ld) { return (LDirent*)(ld + ((LDirent*)ld)->d_reclen); }

int dir_walk_read(DWalkEnv *env)
{
   int bytes_read = syscall(SYS_getdents, env->fd, env->buffer, env->buffer_len);
   if (bytes_read > 0)
   {
      env->data_ptr = (LDirent*)env->buffer;
      env->end_ptr = (LDirent*)(env->buffer + bytes_read);
      return 1;
   }
   else if (bytes_read == 0)
      env->error_state = 0;
   else if (bytes_read == -1)
      env->error_state = errno;

   return 0;
}

EXPORT int ctt_dir_walk_init(DWalkEnv *env, const char *path, char *buffer, int buffer_len)
{
   memset(env, 0, sizeof(DWalkEnv));
   int fd = open(path, O_RDONLY | O_NONBLOCK | O_DIRECTORY);
   if (fd > -1)
   {
      env->dir = path;
      env->buffer = buffer;
      env->buffer_len = buffer_len;

      env->fd = fd;

      return dir_walk_read(env);

   }

   return 0;
}

EXPORT void ctt_dir_walk_release(DWalkEnv *env)
{
   if (env->fd > -1)
   {
      close(env->fd);
      env->fd = 0;
   }
}

EXPORT int ctt_dir_walk(DWalkEnv *env,
                        const char **name,
                        char *type,
                        long *inode)
{
   if (at_buffer_end(env) && !dir_walk_read(env))
      return 0;

   LDirent *pdir = env->data_ptr;
   *name = (const char*)&pdir->d_name;
   *type = *((char*)pdir + pdir->d_reclen - 1);
   *inode = pdir->d_inode;

   env->data_ptr = dirent_increment(pdir);
      
   return 1;
}



#ifdef DIR_WALK_MAIN

int main(int argc, const char **argv)
{
   char buffer[1024];
   DWalkEnv env;

   const char *name;
   char type;
   long inode;

   if (ctt_dir_walk_init(&env, "/usr/bin", buffer, sizeof(buffer)))
   {
      int count = 0;
      while (ctt_dir_walk(&env, &name, &type, &inode))
         printf("%-3d: %s\n", ++count, name);

      ctt_dir_walk_release(&env);
   }
   
   return 0;
}
#endif


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcomment"
// Local Variables:
// compile-command: "base=dir_walk; \
//   gcc -Wall -Werror -ggdb -D${base^^}_MAIN \
//   -DDEBUG -o $base ${base}.c"
// End:
#pragma GCC diagnostic pop
