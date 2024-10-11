#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "parse.h"

int change_mode(char * path, mode_t mode);

int R_flg = 0;
int opterr = 0;		/* Deactivate getopt error message */
char *mode_string = NULL;

int main(int argc, char ** argv)
{
	mode_t mode;
	int retval = 0;

	int opt;
	while((opt = getopt(argc, argv,"R")) != -1) {
		switch(opt) {
		case('R'):
			R_flg++;
			break;
		case('?'):
			if(optopt != 'r' && optopt != 'w' && optopt != 'x' &&
			   optopt != 'X' && optopt != 's' && optopt != 't' ) {
				fprintf(stderr, "invalid option -- %c \n", optopt);
				return 1;
			}
			if (mode_string != NULL) {
				fprintf(stderr, "More than file mode\n");
				return 1;
			}
			mode_string = argv[optind-1];
		}
	}

	if (argc - optind == 0) {
		fprintf(stderr, "Wrong amount of arguments \n");
		return 1;
	}

	if (mode_string == NULL)
		mode_string = argv[optind];

	if (parsemode(mode_string, &mode)) {
		fprintf(stderr, "Couldn't parse given mode\n");
		return 1;
	}

	for (int i = optind + 1; i < argc; i++) {
		if (change_mode(argv[i], mode))
			retval = 1;
	}

	return retval;
}


int change_mode(char * path, mode_t mode)
{
	chmod(path, mode);
	return 0;
}
