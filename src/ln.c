#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static char *progname;
int f_flg = 0;
int s_flg = 0;

int main(int argc, char ** argv)
{
	int retval = 0;
	int flags = 0;
	int working_fd = AT_FDCWD;
	char * target;
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
			return 1;
		}

	argc -= optind;
	argv += optind;

	if (argc == 0 || argc == 1) {
		fprintf(stderr,"%s: operand is missing\nSee the man page for help.\n", progname);
		return 1;
	}

	struct stat sb;
	if ((stat(argv[argc - 1], &sb) == -1) && errno != ENOENT) {
		fprintf(stderr,"%s: failed to stat '%s': %s\n",
				progname, argv[argc - 1], strerror(errno));
	       	return 1;
	}

	if (S_ISDIR(sb.st_mode)) {
		if ((working_fd = open(argv[argc - 1], O_RDONLY)) == -1) {
			fprintf(stderr,"%s: failed to open '%s': %s\n",
				progname, argv[argc - 1], strerror(errno));
			return 1;
		}
	} else if (argc > 3) {
		fprintf(stderr,"%s: invalid usage\nSee the man page for help.\n", progname);
		return 1;
	}

	for (int i = 0; i < argc - 1; i++) {
		if (S_ISDIR(sb.st_mode))
			target = argv[i];
		else
			target = argv[argc - 1];

		if (f_flg && unlinkat(working_fd, target, 0) < 0 && errno != ENOENT) {
			fprintf(stderr, "%s: failed to unlink '%s': %s\n",
				progname, argv[i], strerror(errno));
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

		fprintf(stderr, "%s: failed to link '%s' -> '%s' : %s\n",
			progname, argv[i], argv[argc - 1], strerror(errno));

		retval = 1;
	}

	return retval;
}
