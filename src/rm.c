#include <stdio.h>
#include <getopt.h>
#include <ftw.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "util.h"

int delete(const char *fpath, const struct stat *sb, int tflag, struct FTW * ftwbuf);

static char *progname;
int f_flg = 0;
int i_flg = 0;
int R_flg = 0;
int retval = 0;

int main(int argc, char ** argv)
{
	progname = argv[0];
	struct stat sb;

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
			fprintf(stderr,"See the man page for help.\n");
			return 1;
		}

	argc -= optind;
	argv += optind;

	if (argc == 0) {
		fprintf(stderr,"%s: operand is missing\nSee the man page for help.\n", progname);
		return 1;
	}

	for (int i = 0; i < argc; i++) {
		if (lstat(argv[i], &sb)) {
			if (!(errno == ENOENT && f_flg)) {
				fprintf(stderr, "%s: failed to stat file '%s': %s\n",
						progname, argv[i], strerror(errno));
				retval = 1;
			}
			continue;
		}

		if (R_flg) {
			if (walk(argv[i], delete, 'P'))
				fprintf(stderr, "%s: failed to open file '%s': %s\n",
					progname, argv[i], strerror(errno));
		} else {
			if (S_ISDIR(sb.st_mode)) {
				fprintf(stderr, "%s: -R flag must be specified with directories\n",
						progname);
				retval = 1;
				continue;
			}

			delete(argv[i], &sb, 0, 0);
		}
	}

	return retval;
}

int delete(const char *fpath, const struct stat *sb, int tflag, struct FTW * ftwbuf)
{
	char tmp;
	int respond = 0;

	if (i_flg) {
		fprintf(stderr, "remove %s? ", fpath);
		if (read(0, &tmp, 1) == 1) {
			respond = (tmp == 'y' || tmp == 'Y');
			while (tmp != '\n' && read(0, &tmp, 1) == 1);
		}
		if (!respond)
			return 0;
	}

	if (FTW_D && R_flg) {
		if (!rmdir(fpath))
			return 0;
	} else {
		if (!unlink(fpath))
			return 0;
	}

	fprintf(stderr, "%s: failed to remove file '%s': %s\n", progname, fpath, strerror(errno));
	retval = 1;

	return 0;
}
