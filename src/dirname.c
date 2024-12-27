#include <libgen.h>
#include <stdio.h>

#include "util.h"

int
main(int argc, char **argv)
{
	progname = argv[0];

	if (argc != 2)
		errprintf(1, ":operand is missing\nSee the man page for help");

	printf("%s\n", dirname(argv[1]));
	/* FIXME: Flushing might be necessary */
	return 0;
}
