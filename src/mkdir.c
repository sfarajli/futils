#include <errno.h>
#include <getopt.h>
#include <libgen.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "util.h"

int make_directory(char *, mode_t);

static char *progname;
static int p_flg = 0;
static int v_flg = 0;

int main(int argc, char ** argv)
{
	int retval = 0;
	mode_t mode = 0777;
	progname = argv[0];

	int opt;
	while ((opt = getopt(argc, argv, "pvm:")) != -1)
		switch (opt) {
		case 'p':
			p_flg = 1;
			break;
		case 'v':
			v_flg = 1;
			break;
		case 'm':
			umask(0);
			if (parsemode(optarg, &mode)) {
				fprintf(stderr, "%s: failed to parse given mode '%s'\n", progname, optarg);
				return 1;
			}
			break;
		default:
			return 1;
		}

	argv += optind;
	argc -= optind;

	if (argc == 0) {
		fprintf(stderr,"%s: operand is missing\nSee the man page for help.\n", progname);
		return 1;
	}

	for (int i = 0; i < argc; i++)
		if (make_directory(argv[i], mode))
			retval = 1;

	return retval;
}

int make_directory(char* path, mode_t mode)
{
	if (p_flg) {
		char * buf = strdup(path);
		char * parent_dir= dirname(buf);

		if (strcmp(parent_dir, path) != 0)
			if (make_directory(parent_dir, mode))
				return 1;

		free(buf);
	}

	if (mkdir(path, mode) == 0) {
		if (v_flg)
			printf("%s: directory created '%s'\n",progname, path);
		return 0;
	}

	if (errno == EEXIST && p_flg) {
		struct stat sb;
		stat(path, &sb);
		if ((sb.st_mode & S_IFMT) == S_IFDIR)
			return 0;
	}

	fprintf(stderr, "%s: failed to create directory '%s': %s\n",
				progname, path, strerror(errno));
	return 1;
}
