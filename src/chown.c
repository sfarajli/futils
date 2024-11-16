#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>
#include <ftw.h>
#include <grp.h>
#include <stdlib.h>
#include <limits.h>
#include <pwd.h>
/* TODO: sort alphabetically */

#include "util.h"

int change_owner(const char * fpath, const struct stat * sb, int tflag, struct FTW * ftwbuf);
int parse_owner(char * str); /*TODO: add function definition */

static char *progname;
int chown_flags = 0;
int retval = 0;
uid_t uid;
gid_t gid;

int main(int argc, char ** argv)
{
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

	if (parse_owner(argv[0])) {
		fprintf(stderr, "%s: failed to parse given owner '%s'", progname, argv[0]);
		return 1;
	}

	for (int i = 1 ; i < argc; i++)
		if (R_flg) {
			if (walk(argv[i], change_owner, recurse_mode))
				fprintf(stderr, "%s: failed to open file '%s': %s\n",
					progname, argv[i], strerror(errno));
		} else {
			change_owner(argv[i], NULL, 1, 0);
		}

	return retval;
}

int change_owner(const char *fpath, const struct stat *sb, int tflag, struct FTW * ftwbuf)
{
	if (fchownat(AT_FDCWD, fpath, -1, gid, chown_flags)) {
		fprintf(stderr, "%s: failed to change group '%s': %s\n",
			progname, fpath, strerror(errno));
		retval = 1;
	}
	return 0;
}

int parse(char * str)
{
	char * buf;
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

	if (*str == '\0')
		goto group;

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
