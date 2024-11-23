#include <ftw.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#include "util.h"

#define NOPENFD 3

int
walk(char *path,
               int (*func) (const char *, const struct stat *, int, struct FTW *),
               char mode, int recurse)
{
	char *buf;
	int nftw_flags = FTW_PHYS | FTW_DEPTH;
	struct stat sb;
	struct FTW ftwbuf;

	if (!recurse) {
		if (stat(path, &sb)) {
			errprintf(0, ":failed to stat file '%s'", path);
			return 1;
		}

		ftwbuf.level = 0;
		ftwbuf.base = 0;

		/* TODO: add type flag */
		(void)func(path, &sb, 0, &ftwbuf);

		return 1;
	}


	switch (mode) {
	case 'L':
		nftw_flags &= ~FTW_PHYS;
		break;
	case 'H':
		if ((buf = realpath(path, NULL)) == NULL) { /* Dereference only the path argument for H flag */
			errprintf(0, ":failed to dereference file '%s'", path);
			return 1;
		}

		path = buf;
		/* FALLTHROUGH */
	case 'P':
		nftw_flags |= FTW_PHYS;
	}

	if (nftw(path, func, NOPENFD, nftw_flags)) {
		errprintf(0, ":failed to walk file '%s'", path);
		return 1;
	}

	return 0;
}
