#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/stat.h>
#include <unistd.h>

#include "util.h"

static int f_flg = 0;
static int s_flg = 0;

int
main(int argc, char **argv)
{
	struct stat sb;
	int working_fd = AT_FDCWD;
	int retval = 0;
	int flags = 0;
	char *target;
	progname = argv[0];

	int opt;
	while ((opt = getopt(argc, argv, "fLPs")) != -1)
		switch (opt) {
		case 'f':
			f_flg = 1;
			break;
		case 'L':
			flags |= AT_SYMLINK_FOLLOW;
			break;
		case 'P':
			flags &= ~AT_SYMLINK_FOLLOW;
			break;
		case 's':
			s_flg = 1;
			break;
		default:
			errprintf(1, "See the man page for help.");
		}

	argc -= optind;
	argv += optind;

	if (argc == 0 || argc == 1)
		errprintf(1, ":operand is missing\nSee the man page for help");

	if ((stat(argv[argc - 1], &sb) == -1) && errno != ENOENT)
		errprintf(1, ":failed to stat '%s':", argv[argc - 1]);

	if (S_ISDIR(sb.st_mode)) {
		if ((working_fd = open(argv[argc - 1], O_RDONLY)) == -1)
			errprintf(1, ":failed to open '%s':");
	} else if (argc > 2)
		errprintf(1, ":invalid usage\nSee the man page for help");

	for (int i = 0; i < argc - 1; i++) {
		if (S_ISDIR(sb.st_mode))
			target = argv[i];
		else
			target = argv[argc - 1];

		if (f_flg && unlinkat(working_fd, target, 0) < 0 && errno != ENOENT) {
			errprintf(0, ":failed to unlink '%s':", argv[i]);
			retval = 1;
			continue;
		}

		if (s_flg) {
		       	if (!symlinkat(argv[i], working_fd, target))
				continue;
		} else {
			if (!linkat(AT_FDCWD, argv[i], working_fd, target, flags))
				continue;
		}

		errprintf(0, ":failed to link '%s' -> '%s':", argv[i], argv[argc - 1]);
		retval = 1;
	}

	return retval;
}
