#include <ftw.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#include "util.h"

#define NOPENFD 3

void
walk(char * path,
               int (*func) (const char *, const struct stat *, int, struct FTW *),
               char mode, int recurse)
{
	int nftw_flags = FTW_PHYS;
	struct stat sb;
	struct FTW ftwbuf;

	if (!recurse) {
		if (stat(path, &sb)) {
			fprintf(stderr, "failed to stat given file %s\n", path);
			return;
		}

		ftwbuf.level = 0;
		ftwbuf.base = 0;

		/* TODO: add type flag */
		(void)func(path, &sb, 0, &ftwbuf);

		return;
	}


	switch (mode) {
	case 'L':
		nftw_flags &= ~FTW_PHYS;
		break;
	case 'H':
		if ((path = realpath(path, NULL)) == NULL) { /* Dereference only the path argument for H flag */
			fprintf(stderr, "failed to dereference given file '%s'", path);
			return;
		}
		/* FALLTHROUGH */
	case 'P':
		nftw_flags |= FTW_PHYS;
	}

	if (nftw(path, func, NOPENFD, nftw_flags))
		fprintf(stderr, "failed to walk file '%s'\n", path);

	return;
}
