#include <libgen.h>
#include <string.h>
#include <unistd.h>

#include "util.h"

static int remove_directory(char *);

static int p_flg = 0;

int
main(int argc, char **argv)
{
	int retval = 0;
	progname = argv[0];

	int opt;
	while ((opt = getopt(argc, argv, "p")) != -1)
		switch (opt) {
		case 'p':
			p_flg = 1;
			break;
		default:
			errprintf(1, "See the man page for help.");
		}

	argc -= optind;
	argv += optind;

	if (argc == 0)
		errprintf(1, ":operand is missing\nSee the man page for help");

	for (int i = 0; i < argc; i++)
		if (remove_directory(argv[i]))
			retval = 1;

	return retval;
}

int
remove_directory(char *path)
{
	char *parent_dir;
	if (rmdir(path)) {
		errprintf(0, ":failed to remove directory '%s':", path);
		return 1;
	}

	if (p_flg) {
		parent_dir = dirname(path);
		if ((strcmp(parent_dir, "/" ) != 0) && (strcmp(parent_dir, "." ) != 0))
			if (remove_directory(parent_dir))
				return 1;
	}

	return 0;
}
