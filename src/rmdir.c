#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>

int remove_directory(char *);

static char *progname;
int p_flg = 0;

int main(int argc, char ** argv)
{
	progname = argv[0];
	int retval = 0;

	int opt;
	while ((opt = getopt(argc, argv, "p")) != -1)
		switch (opt) {
		case 'p':
			p_flg = 1;
			break;
		default:
			return 1;
		}

	argc -= optind;
	argv += optind;

	if (argc == 0) {
		fprintf(stderr,"%s: operand is missing\nSee the man page for help.\n", progname);
		return 1;
	}

	for (int i = 0; i < argc; i++)
		if (remove_directory(argv[i]))
			retval = 1;

	return retval;
}

int remove_directory(char * path)
{
	if (rmdir(path)) {
		fprintf(stderr, "%s: failed to remove directory '%s': %s\n",
			progname, path, strerror(errno));
		return 1;
	}

	if (p_flg) {
		char * parent_dir = dirname(path);
		if ((strcmp(parent_dir, "/" ) != 0) || (strcmp(parent_dir, "." ) != 0))
			if (remove_directory(parent_dir))
				return 1;
	}

	return 0;
}
