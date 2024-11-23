#include <fcntl.h>
#include <ftw.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "util.h"

static int change_mode(const char *, const struct stat *, int, struct FTW *);

static int retval = 0;
static char *mode_str = NULL;

int
main(int argc, char **argv)
{
	char *tmp;
	int R_flg = 0;
	progname = argv[0];

	int opt;
	while ((opt = getopt(argc, argv,"RrwxXst")) != -1) {
		switch(opt) {
		case 'R':
			R_flg++;
			break;
		case 'r':
		case 'w':
		case 'x':
		case 'X':
		case 's':
		case 't':
			if (mode_str)
				errprintf(1, ":more than one mode given");
				/* FIXME: output better error message */

			tmp = argv[optind - 1];
			if (tmp[strlen(tmp) - 1] == opt)
				mode_str = argv[optind - 1];
			break;
		default:
			errprintf(1, "See the man page for help.");
		}
	}

	argc -= optind;
	argv += optind;

	/* FIXME: ./chmod without any argument doesn't give an error */
	if (!mode_str) {
		mode_str = argv[0];
		argc--;
		argv++;
	}

	if (argc == 0)
		errprintf(1, ":operand is missing\nSee the man page for help");

	/* FIXME: handle umask better*/
	umask(0);
	for (int i = 0; i < argc; i++)
		if (walk(argv[i], change_mode, 'H', R_flg))
			retval = 1;

	return retval;
}

int
change_mode(const char *path, const struct stat *sb, int tflag, struct FTW *ftwbuf)
{
	mode_t mode = sb->st_mode;

	if (parsemode(mode_str, &mode)) {
		errprintf(0, ":failed to parse given mode '%s'", mode_str);
		retval = 1;
	}

	if (fchmodat(AT_FDCWD, path, mode, 0)) {
		errprintf(0, ":failed to change mode '%s':", path);
			/* FIXME: output better diagnostics message */
		retval = 1;
	}

	return 0; /* Continue traversing */
}
