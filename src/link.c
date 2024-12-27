#include <unistd.h>

#include "util.h"

int
main(int argc, char **argv)
{
	progname = argv[0];

	if (argc != 3)
		errprintf(1, ":Invalid usage\nSee the man page for help");

	if (link(argv[1], argv[2]))
		errprintf(1, ":failed to link '%s' -> '%s':", argv[2], argv[1]);

	return 0;
}
