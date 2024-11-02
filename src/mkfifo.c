#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>

#include "parse.h"

static char *progname;

int main(int argc, char ** argv)
{
	int retval = 0;
	mode_t mode = 0666;
	progname = argv[0];

	int opt;
	while ((opt = getopt(argc, argv, "m:")) != -1)
		switch (opt) {
		case 'm':
			umask(0);
			if (parsemode(optarg, &mode)) {
				fprintf(stderr, "%s: failed to parse given mode '%s'\n",
						progname, optarg);
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

	for (int i = 0; i < argc; i++) {
		if (mkfifo(argv[i], mode)) {
			fprintf(stderr, "%s: failed to create fifo '%s': %s\n",
				progname, argv[i], strerror(errno));

			retval = 1;
		}
	}

	return retval;
}
