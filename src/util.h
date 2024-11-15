#ifndef _LIBUTIL_H__
#define _LIBUTIL_H__

#include <sys/types.h>
#include <ftw.h>

int parsemode(char * str, mode_t * mode);

int walk(char * path,
               int (*) (const char * fpath, const struct stat *sb, int tflag, struct FTW * ftwbuf),
               char mode);

#endif
