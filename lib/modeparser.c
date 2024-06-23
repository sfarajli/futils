#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "parse.h"


int parsemode(char * str, mode_t * mode)
{
	char * end;
	mode_t octal;
	int who;
	int perm;
	char sign;

	octal = strtol(str, &end, 8);
	if (*end == '\0') {
		if (octal > 0777)
			return 1;

		* mode = octal;
		return 0;
	}

again:
	who = 0;
	for (;; str++) {
		switch(*str) {
		case('a'):
			who = 0777;
			continue;

		case('u'):
			who |= S_IRUSR | S_IWUSR | S_IXUSR;
			continue;

		case('g'):
			who |= S_IRGRP | S_IWGRP | S_IXGRP;
			continue;

		case('o'):
			who |= S_IROTH | S_IWOTH | S_IXOTH;
			continue;
		}
		break;
	}

	if (who == 0)
		who = 0777;

next:
	perm = 0;
	sign = *str++;

	for(; *str; str++) {
		switch(*str) {
		case('r'):
			perm |= S_IRUSR | S_IRGRP | S_IROTH;
			continue;

		case('w'):
			perm |= S_IWUSR | S_IWGRP | S_IWOTH;
			continue;

		case('x'):
			perm |= S_IXUSR | S_IXGRP | S_IXOTH;
			continue;

		case('u'):
			if(*mode & S_IRUSR)
				perm |= S_IRUSR | S_IRGRP | S_IROTH;

			if(*mode & S_IWUSR)
				perm |= S_IWUSR | S_IWGRP | S_IWOTH;

			if(*mode & S_IXUSR)
				perm |= S_IXUSR | S_IXGRP | S_IXOTH;

			continue;


		case('g'):
			if(*mode & S_IRGRP)
				perm |= S_IRUSR | S_IRGRP | S_IROTH;

			if(*mode & S_IWGRP)
				perm |= S_IWUSR | S_IWGRP | S_IWOTH;

			if(*mode & S_IXGRP)
				perm |= S_IXUSR | S_IXGRP | S_IXOTH;

			continue;

		case('o'):
			if(*mode & S_IROTH)
				perm |= S_IRUSR | S_IRGRP | S_IROTH;

			if(*mode & S_IWOTH)
				perm |= S_IWUSR | S_IWGRP | S_IWOTH;

			if(*mode & S_IXOTH)
				perm |= S_IXUSR | S_IXGRP | S_IXOTH;

			continue;
		}
		break;
	}


	switch (sign) {
		case('+'):
			* mode |= perm & who;
			break;

		case('-'):
			* mode &= ~(perm & who);
			break;

		case('='):
			*mode &= ~who; 		/* clear the bits of target user */
			*mode |= who & perm;

			break;

		default:
			return 1;
	}

	switch (*str) {
		case('\0'):
			return 0;

		case(','):
			goto again;

		case('+'):	/* FALLTHROUGH */
		case('='):	/* FALLTHROUGH */
		case('-'):	/* FALLTHROUGH */
			goto next;
		}

	return 1;
}
