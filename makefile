#1. no implicit rules should be used
#2. binaries should be on the bin directory
#3. directory layout should be somewhat proper
#4. as compact and simple as possible
# $(info $(obj))

VPATH = src

binaries = bin/chmod \
	   bin/touch \
	   bin/mkdir \

all: $(binaries)
	
$(binaries): bin/%: %.o | bin
	cc -o $@ $^

%.o: src/%.c
	cc -c -o $@ $<


bin:
	mkdir bin
clean: 
	rm -rf *.o bin

.PHONY: all clear

bin/chmod bin/mkdir: modeparser.o
chmod.o modeparser.o: parse.h
