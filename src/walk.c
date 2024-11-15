#include <ftw.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

#include "util.h"

int walk(char * path,
               int (*func) (const char * fpath, const struct stat *sb, int tflag, struct FTW * ftwbuf),
               char mode)
{
	int flags = FTW_PHYS;
	errno = 0;

	switch (mode) {
	case 'L':
		flags &= ~FTW_PHYS;
		break;
	case 'H': /* FALLTHROUGH */
		if ((path = realpath(path, NULL)) == NULL) /* Dereference only the path argument */
			return 1;
	case 'P':
		flags |= FTW_PHYS;
	}

	if (nftw(path, func, 3, flags))
		return 1;

	return 0;
}
