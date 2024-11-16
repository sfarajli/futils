#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>
#include <ftw.h>
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
