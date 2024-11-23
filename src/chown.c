#include <errno.h>
#include <fcntl.h>
#include <ftw.h>
#include <getopt.h>
#include <grp.h>
#include <limits.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "util.h"

static int change_owner(const char *, const struct stat *, int, struct FTW *);
static int parse_owner(char *);

static uid_t uid;
static gid_t gid;
static int chown_flags = 0;
static int retval = 0;
static int R_flg = 0;
static int h_flg = 0;

int
main(int argc, char **argv)
{
	char recurse_mode = 'P'; 	/* Default recursion mode */
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

	if (parse_owner(argv[0]))
		errprintf(1, ":failed to parse owner '%s'", argv[0]);

	for (int i = 1; i < argc; i++)
		if (walk(argv[i], change_owner, recurse_mode, R_flg))
			retval = 1;

	return retval;
}

int
change_owner(const char *path, const struct stat *sb, int tflag, struct FTW *ftwbuf)
{
	if (fchownat(AT_FDCWD, path, uid, gid, chown_flags)) {
		errprintf(0, ":failed to change group '%s':", path);
		retval = 1;
	}

	return 0; /* Continue traversing */
}

int
parse_owner(char *str)
{
	char *buf;
	struct group *gp;
	struct passwd *pw;
	int length = strlen(str);
	int i;

	for (i = 0; i < length; i++) {
		if (str[i] == ':') {
			str[i] = '\0';
			break;
		}
	}

	if (*str == '\0') {
		uid = -1;
		goto group;
	}

	errno = 0;
	if ((pw = getpwnam(str))) {
		uid = pw->pw_uid;
	} else {
		if (errno)
			return 1;
		uid = strtol(str, &buf, 10);
		if (*buf != '\0' || uid >= UINT_MAX)
			return 1;
	}

group:
	if (i == length || i == length - 1) {
		if (*str == '\0')		/* ':' or empty string cases */
			return 1;

		gid = -1;
		return 0;			/* 'user:' and 'user' cases*/
	}

	str += i + 1;

	errno = 0;
	if ((gp = getgrnam(str))) {
		gid = gp->gr_gid;
	} else {
		if (errno)
			return 1;

		gid = strtol(str, &buf, 10);
		if (*buf != '\0' || gid >= UINT_MAX)
			return 1;
	}

	return 0;
}
