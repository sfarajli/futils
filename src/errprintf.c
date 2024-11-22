#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

char *progname = NULL;

void
errprintf(const int errval, char *msg, ...)
{
	va_list ap;

	if (progname && msg[0] == ':') {
		fprintf(stderr, "%s: ", progname);
		msg++;
	}

	va_start(ap, msg);
	vfprintf(stderr, msg, ap);
	va_end(ap);

	if (msg[0] && msg[strlen(msg)-1] == ':') {
		fputc(' ', stderr);
		perror(NULL);
	} else {
		putchar('\n');
	}

	if (errval != 0)
		exit(errval);
}
