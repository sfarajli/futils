#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <grp.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
/*TODO: Sort alphabetically*/

static char *progname;

int h_flg = 0;
int R_flg = 0;
int H_flg = 0;
int L_flg = 0;
int P_flg = 0;

int change_group(char *, gid_t);

int main(int argc, char ** argv)
{
	int retval = 0;
	gid_t gid;
	struct group *gp;
	progname = argv[0];

	int opt;
	while ((opt = getopt(argc, argv, "hRHLP")) != -1)

		switch (opt) {
		case 'h':
			h_flg = 1;
			break;
		case 'R':
			R_flg = 1;
			break;
		case 'H':
			H_flg = 1;
			break;
		case 'L':
			L_flg = 1;
			break;
		case 'P':
			P_flg = 1;
			break;
		default:
			fprintf(stderr,"See the man page for help.\n");
			return 1;
		}

	argc -= optind;
	argv += optind;

	if (argc == 0 || argc == 1) {
		fprintf(stderr,"%s: operand is missing\nSee the man page for help.\n", progname);
		return 1;
	}

	errno = 0;
	if ((gp = getgrnam(argv[0]))) {
		gid = gp->gr_gid;
	} else {
		if (errno){
			fprintf(stderr,"%s: failed to get group name '%s': %s\n",
					progname, argv[0], strerror(errno));
			return 1;
		}

		char * buf;
		gid = strtol(argv[0], &buf, 10);
		if (*buf != '\0' || gid >= UINT_MAX) {
			fprintf(stderr,"%s: invalid group '%s'\n",progname, argv[0]);
			return 1;
		}
	}

	for (int i = 1 ; i < argc; i++)
		if(change_group(argv[i], gid))
			retval = 1;

	return retval;
}

int change_group(char * path, gid_t gid)
{
	/*TODO: Add recursion*/
	struct stat sb;
	if (stat(path, &sb) == -1) {
		fprintf(stderr,"%s: failed to stat '%s': %s\n", progname, path, strerror(errno));
	       	return 1;
	}

	if (chown(path, sb.st_uid, gid)) {
		fprintf(stderr, "%s: failed to change group '%s': %s\n",
			progname, path, strerror(errno));
		return 1;
	}

	return 0;
}
