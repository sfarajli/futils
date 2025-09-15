.POSIX:

BIN = chmod mkdir sync ln rmdir mkfifo chgrp chown rm mv basename dirname false true link unlink

all: ${BIN}

include Sourcedeps
include config.mk

${BIN}:
	${CC} ${LDFLAGS} -o $@ $^

.c.o:
	${CC} ${CFLAGS} -c -o $@ $<

clean:
	rm -f src/*.o ${BIN}

.PHONY: all clean
