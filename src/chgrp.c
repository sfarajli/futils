#include <errno.h>
#include <fcntl.h>
#include <ftw.h>
#include <getopt.h>
#include <grp.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "util.h"

int change_group(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf);

static char *progname;
int chown_flags = 0;
int retval = 0;
gid_t gid;

int main(int argc, char ** argv)
{
	struct group *gp;
	progname = argv[0];
	char recurse_mode = 'P'; 	/* Default recursion mode */
	int R_flg = 0;
	int h_flg = 0;

	int opt;
	while ((opt = getopt(argc, argv, "hRHLP")) != -1)
		switch (opt) {
		case 'h':
			h_flg = 1;
			chown_flags |= AT_SYMLINK_NOFOLLOW;
			break;
		case 'R':
			R_flg = 1;
			break;
		case 'H':
			recurse_mode = 'H';
			h_flg = 0;
			break;
		case 'L':
			recurse_mode = 'L';
			h_flg = 0;
			break;
		case 'P':
			recurse_mode = 'P';
			h_flg = 0;
			break;
		default:
			fprintf(stderr,"See the man page for help.\n");
			return 1;
		}

	if (R_flg && !h_flg) {
		switch (recurse_mode) {
		case 'L':
			chown_flags &= ~AT_SYMLINK_NOFOLLOW;
			break;
		case 'H': /* FALLTHROUGH */
		case 'P':
			chown_flags |= AT_SYMLINK_NOFOLLOW;
		}
	}

	argc -= optind;
	argv += optind;

	if (argc == 0 || argc == 1) {
		fprintf(stderr,"%s: operand is missing\nSee the man page for help.\n", progname);
		return 1;
	}

	errno = 0;
	if ((gp = getgrnam(argv[0]))) {
		gid = gp->gr_gid;
	} else {
		if (errno) {
			fprintf(stderr,"%s: failed to get group name '%s': %s\n",
					progname, argv[0], strerror(errno));
			return 1;
		}

		char * buf;
		gid = strtol(argv[0], &buf, 10);
		if (*buf != '\0' || gid >= UINT_MAX) {
			fprintf(stderr,"%s: invalid group '%s'\n",progname, argv[0]);
			return 1;
		}
	}

	for (int i = 1 ; i < argc; i++)
		if (R_flg) {
			if (walk(argv[i], change_group, recurse_mode))
				fprintf(stderr, "%s: failed to open file '%s': %s\n",
					progname, argv[i], strerror(errno));
		} else {
			change_group(argv[i], NULL, 1, 0);
		}

	return retval;
}

int change_group(const char *fpath, const struct stat *sb, int tflag, struct FTW * ftwbuf)
{
	if (fchownat(AT_FDCWD, fpath, -1, gid, chown_flags)) {
		fprintf(stderr, "%s: failed to change group '%s': %s\n",
			progname, fpath, strerror(errno));
		retval = 1;
	}
	return 0;
}
