#include <libgen.h>
#include <stdio.h>
#include <string.h>

#include "util.h"

int
main(int argc, char **argv)
{
	int x;
	int y;
	char *suffix = NULL;
	char *base;
	progname = argv[0];

	if (argc == 0)
		errprintf(1, ":operand is missing\nSee the man page for help");
	else if (argc == 3)
		suffix = argv[2];
	else if (argc != 2)
		errprintf(1, ":invalid usage\nSee the man page for help");

	base = basename(argv[1]);

	if (suffix == NULL) {
		printf("%s\n", base);
		return 0;
	}

	for  (x = strlen(base), y = strlen(suffix); base[x] == suffix[y]; y--, x--) {
		if (y == 0) {
			if (x != 0)
				base[x] = '\0';
			break;
		}
	}

	printf("%s\n", base);
	return 0;
}
