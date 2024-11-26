#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "util.h"

int
main(int argc, char **argv)
{
	int retval = 0;
	mode_t mode = 0666;
	progname = argv[0];

	int opt;
	while ((opt = getopt(argc, argv, "m:")) != -1)
		switch (opt) {
		case 'm':
			umask(0);
			if (parsemode(optarg, &mode))
				errprintf(1, ":failed to parse given mode '%s'", optarg);
			break;
		default:
			errprintf(1, "See the man page for help.");
		}

	argv += optind;
	argc -= optind;

	if (argc == 0)
		errprintf(1, ":operand is missing\nSee the man page for help");

	for (int i = 0; i < argc; i++) {
		if (mkfifo(argv[i], mode)) {
			errprintf(0, ":failed to create fifo '%s':", argv[i]);
			retval = 1;
		}
	}

	return retval;
}
