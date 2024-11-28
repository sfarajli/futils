#include <unistd.h>

#include "util.h"

int
main(int argc, char ** argv)
{
	int retval = 0;
	progname = argv[0];

	for (int i = 1; i < argc; i++) {
		if (unlink(argv[i])) {
			errprintf(0, ":failed to unlink file '%s':", argv[i]);
			retval = 1;
		}
	}

	return retval;
}
