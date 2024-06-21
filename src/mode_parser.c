#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>


int mode_parser(char * str, mode_t * mode)
{
	char * end;
	mode_t octal;
	int person = 0;
	int perm = 0;
	char sign;

	octal = strtol(str, &end, 8);
	if (*end == '\0') {
		if (octal > 0777 || octal < 0)
			return 1;

		* mode = octal;
		return 0;
	}


again:
	for (;; str++) {
		switch(*str) {
		case('a'):
			person = 0777;
			continue;

		case('u'):
			person |= S_IRUSR | S_IWUSR | S_IXUSR;
			continue;

		case('g'):
			person |= S_IRGRP | S_IWGRP | S_IXGRP;
			continue;

		case('o'):
			person |= S_IROTH | S_IWOTH | S_IXOTH;
			continue;
		}
		break;
	}

	if (person == 0)
		person = 0777;

next:
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
			perm |= *mode | S_IRUSR | S_IWUSR | S_IXUSR;
			continue;

		case('g'):
			perm |= *mode | S_IRGRP | S_IWGRP | S_IXGRP;
			continue;

		case('o'):
			perm |= *mode | S_IROTH | S_IWOTH | S_IXOTH;
			continue;
		}
		break;
	}


	switch (sign) {
		case('+'):
			* mode |= perm | person;
			break;

		case('-'):
			* mode &= ~(perm | person);
			break;

		case('='):
			* mode = perm | person;
			break;

		default:
			return 1;
	}

	switch (*str) {
		case('\0'):
			return 0;

		case(','):
			goto again;

		case('+'):				/* FALLTHROUGH */
		case('='):				/* FALLTHROUGH */
		case('-'):				/* FALLTHROUGH */
			goto next;
		}

}
