#include <getopt.h>
#include <stdio.h>
#include <sys/stat.h>

#include "parse.h"

int make_directory(char *, mode_t);

int p_flg = 0;

int main(int argc, char ** argv) 
{
	int retval = 0;
	mode_t mode = 0777;

	int opt;
	while ((opt = getopt(argc, argv, "pm:")) != -1)
		switch (opt) {
		case 'p':
			p_flg = 1;
			break;
		case 'm':
			umask(0);
			if (parsemode(optarg, &mode)) {
				fprintf(stderr, "couldn't parse given mode `%s`\n", optarg);
				return 1;
			}
			break;
		default: 
			return 1;
		}

	argv += optind;
	argc -= optind;

	for (int i = 0; i < argc; i++)
		if (make_directory(argv[i], mode)) {
			fprintf(stderr, "failed to create directory '%s'\n", argv[i]);
			retval = 1;
		}

	return retval;
}

int make_directory(char * pathname, mode_t mode) 
{
	mkdir(pathname, mode);
	return 0;
}
