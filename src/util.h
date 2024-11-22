#ifndef _LIBUTIL_H__
#define _LIBUTIL_H__

#include <sys/types.h>
#include <ftw.h>

int parsemode(char * str, mode_t * mode);

void walk(char * path,
               int (* func) (const char * path, const struct stat *sb, int tflag, struct FTW * ftwbuf),
               char mode, int recurse);

#endif
