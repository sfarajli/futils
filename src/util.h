#ifndef _LIBUTIL_H__
#define _LIBUTIL_H__

#include <sys/types.h>
#include <ftw.h>
#include <stddef.h>

extern char * progname;

int parsemode(char * str, mode_t * mode);
void errprintf(const int errval, char *msg, ...);

void walk(char * path,
               int (* func) (const char * path, const struct stat *sb, int tflag, struct FTW * ftwbuf),
               char mode, int recurse);

#endif
