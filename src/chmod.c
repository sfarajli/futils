#include <errno.h>
#include <fcntl.h>
#include <ftw.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "util.h"

int change_mod(const char *fpath, const struct stat *sb, int tflag, struct FTW * ftwbuf);

static char * progname;
char * mode_str = NULL;
int retval = 0;

int main(int argc, char ** argv)
{
	progname = argv[0];
	int R_flg = 0;

	char * tmp;
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
			if (mode_str) {
				fprintf(stderr, "%s: more than one mode given\n", progname);
				/* FIXME: output better error message */
				return 1;
			}
			tmp = argv[optind - 1];
			if (tmp[strlen(tmp) - 1] == opt)
				mode_str = argv[optind - 1];
			break;
		default:
			fprintf(stderr, "See the man page for help.\n");
			return 1;
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

	if (argc == 0) {
		fprintf(stderr,"%s: operand is missing\nSee the man page for help.\n", progname);
		return 1;
	}

	umask(0);
	for (int i = 0; i < argc; i++)
		walk(argv[i], change_mod, 'H', R_flg);

	return retval;
}

int change_mod(const char *fpath, const struct stat *sb, int tflag, struct FTW * ftwbuf)
{
	mode_t mode = sb->st_mode;
	if (parsemode(mode_str, &mode)) {
		fprintf(stderr, "%s: failed to parse given mode '%s'\n",
				progname, mode_str);
		retval = 1;
	}

	if (fchmodat(AT_FDCWD, fpath, mode, 0)) {
		fprintf(stderr, "%s: failed to change mode '%s': %s\n",
			progname, fpath, strerror(errno));
		retval = 1;
	}

	return 0;
}
