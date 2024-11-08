# Fully posix complaint makefile

.POSIX:

.ALLSRC ?= $^
.TARGET ?= $@

BIN= chmod mkdir sync ln rmdir mkfifo chgrp

all: $(BIN)

include Sourcedeps
include config.mk

$(BIN):
	${CC} ${LDFLAGS} -o ${.TARGET} ${.ALLSRC}

.c.o:
	${CC} ${CFLAGS} -c -o ${.TARGET} $<

clean:
	rm -f src/*.o $(BIN)

.PHONY: all
