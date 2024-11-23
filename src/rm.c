#include <errno.h>
#include <ftw.h>
#include <getopt.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "util.h"

static int delete(const char *, const struct stat *, int, struct FTW *);

static int retval = 0;

int
main(int argc, char **argv)
{
	char tmp;
	struct stat sb;
	int respond = 0;
	int f_flg = 0;
	int i_flg = 0;
	int R_flg = 0;
	progname = argv[0];

	int opt;
	while ((opt = getopt(argc, argv, "fiRr")) != -1)
		switch (opt) {
		case 'f':
			f_flg = 1;
			i_flg = 0;
			break;
		case 'i':
			i_flg = 1;
			f_flg = 0;
			break;
		case 'r': /* FALLTHROUGH */
		case 'R':
			R_flg = 1;
			break;
		default:
			errprintf(1, "See the man page for help.");
		}

	argc -= optind;
	argv += optind;

	if (argc == 0)
		errprintf(1, ":operand is missing\nSee the man page for help.");

	for (int i = 0; i < argc; i++) {
		if (lstat(argv[i], &sb)) {
			if (errno == ENOENT && !f_flg) {
				errprintf(0, ":failed to stat file '%s'", argv[i]);
				retval = 1;
			}
			continue;
		}

		if (S_ISDIR(sb.st_mode) && !R_flg) {
			errprintf(0, ":-R flag must be present with directories '%s'", argv[i]);
			retval = 1;
			continue;
		}

		/* TODO: add non-writable check as well */
		if (i_flg) {
			fprintf(stderr, "remove %s? ", argv[i]);
			if (read(0, &tmp, 1) == 1) {
				respond = (tmp == 'y' || tmp == 'Y');
				while (tmp != '\n' && read(0, &tmp, 1) == 1);
			}
			if (!respond)
				return 0;
		}

		if (walk(argv[i], delete, 'P', R_flg))
			retval = 1;
	}

	return retval;
}

int
delete(const char *path, const struct stat *sb, int tflag, struct FTW * ftwbuf)
{
	if (S_ISDIR(sb->st_mode)) {
		if (!rmdir(path))
			return 0;
	} else if (!unlink(path)) {
			return 0;
	}

	errprintf(0, ":failed to remove file '%s':", path);
	retval = 1;

	return 0; /* Continue traversing */
}
