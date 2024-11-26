#include <stdio.h>
#include <ftw.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
/* TODO: sort alphabetically */

#include "util.h"

static int move(const char *, const struct stat *, int, struct FTW *);

static int retval = 0;
static int working_fd = AT_FDCWD;
static char *target;

int
main(int argc, char **argv)
{
	struct stat sb = {0};
	char *lastarg;
	int f_flg = 0;
	int i_flg = 0;
	int isdir = 0;
	progname = argv[0];
	/* TODO: add -i and -f flag mechanisms */

	int opt;
	while ((opt = getopt(argc, argv, "if")) != -1)
		switch (opt) {
		case 'i':
			i_flg = 1;
			f_flg = 0;
			break;
		case 'f':
			f_flg = 1;
			i_flg = 0;
			break;
		default:
			errprintf(1, "See the man page for help.");
		}

	argc -= optind;
	argv += optind;

	if (argc == 0 || argc == 1)
		errprintf(1, ":operand is missing\nSee the man page for help");

	lastarg = argv[argc - 1];

	if (stat(lastarg, &sb) == -1) {
		if (errno != ENOENT)
			errprintf(1, ":failed to stat '%s':", lastarg);
		if (argc > 2)
			errprintf(1, ":invalid usage\nSee the man page for help");

	} else if ((S_ISDIR(sb.st_mode)) || (lastarg[strlen(lastarg) - 1] == '/')) {
		if ((working_fd = open(lastarg, O_RDONLY)) == -1)
			errprintf(1, ":failed to open '%s':", lastarg);
		isdir = 1;
	}

	target = lastarg; 	/* For non-directory */
	for (int i = 0; i < argc - 1; i++) {
		if (isdir)
			target = argv[i];

		if (walk(argv[i], move, 'P', 1))
			retval = 1;
	}

	return 0;
}

int
move (const char *path, const struct stat *sb, int tflag, struct FTW *ftwbuf)
{
	if (renameat(AT_FDCWD, path, working_fd, target)) {
		errprintf(0, ":failed to move '%s' to '%s':", path, target);
		retval = 1;
	}

	return 0;  /* Continue traversing */
}
