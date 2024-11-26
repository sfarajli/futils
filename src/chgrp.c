#include <errno.h>
#include <fcntl.h>
#include <ftw.h>
#include <grp.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "util.h"

static int change_group(const char *, const struct stat *, int, struct FTW *);

static int chown_flags = 0;
static int retval = 0;
static gid_t gid;

int
main(int argc, char **argv)
{
	int R_flg = 0;
	int h_flg = 0;
	char recurse_mode = 'P'; 	/* Default recursion mode */
	struct group *gp;
	progname = argv[0];

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
		case 'H': /* FALLTHROUGH */
		case 'L': /* FALLTHROUGH */
		case 'P':
			recurse_mode = opt;
			h_flg = 0;
			break;
		default:
			errprintf(1, "See the man page for help.");
		}

	argc -= optind;
	argv += optind;


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

	if (argc == 0 || argc == 1)
		errprintf(1, ":operand is missing\nSee the man page for help");

	errno = 0;
	if ((gp = getgrnam(argv[0]))) {
		gid = gp->gr_gid;
	} else {
		if (errno)
			errprintf(1, ":failed to get group name '%s':", argv[0]);

		char *buf;
		gid = strtol(argv[0], &buf, 10);

		if (*buf != '\0' || gid >= UINT_MAX)
			errprintf(1, ":invalid group '%s':", argv[0]);
	}

	for (int i = 1 ; i < argc; i++)
		if (walk(argv[i], change_group, recurse_mode, R_flg))
			retval = 1;

	return retval;
}

int
change_group(const char *path, const struct stat *sb, int tflag, struct FTW *ftwbuf)
{
	if (fchownat(AT_FDCWD, path, -1, gid, chown_flags)) {
		errprintf(0, ":failed to change group '%s':", path);
		retval = 1;
	}

	return 0; /* Continue traversing */
}
