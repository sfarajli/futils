CFLAGS   = -std=c99 -pedantic -Wall -Wno-deprecated-declarations -O2 -D_XOPEN_SOURCE=600

PREFIX 	  ?= /usr/local
BINPREFIX ?= $(PREFIX)/bin

VPATH = src

BIN = bin/chmod \
      bin/touch \


all: $(BIN)

$(BIN): bin/%: %.o | bin
	$(CC) $(CFLAGS) -o $@ $^

%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

install: $(BIN)
	mkdir -p $(DESTDIR)$(BINPREFIX)
	cp -p bin/* $(DESTDIR)$(BINPREFIX)

uninstall: 
	rm -f $(patsubst bin/%, $(DESTDIR)$(BINPREFIX)/%, $(BIN))

bin:
	mkdir $@

clean: 
	rm -rf *.o bin

.PHONY: all clear install uninstall

bin/chmod: modeparser.o
chmod.o modeparser.o: parse.h
