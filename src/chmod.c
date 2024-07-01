#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "parse.h"

int change_mode(char * path, mode_t mode);

int R_flg = 0;
int opterr = 0;		/* Deactivate getopt error message */

int main(int argc, char ** argv)
{
	mode_t mode;
	int retval = 0;

	int opt;
	while((opt = getopt(argc, argv,"R")) != -1) {
		switch(opt) {
		case('R'):
			R_flg++;
		case('?'):
			if(optopt != 'r' && optopt != 'w' && optopt != 'x' &&
			   optopt != 'X' && optopt != 's' && optopt != 't' ) {
				fprintf(stderr, "invalid option -- %c \n", optopt);
				return 1;
			}
		}
	}

	if (parsemode(argv[optind], &mode)) {
		fprintf(stderr, "Couldn't parse given mode\n");
		return 1;
	}

	if (argc - optind == 0)
		fprintf(stderr, "Wrong amount of arguments \n");

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
