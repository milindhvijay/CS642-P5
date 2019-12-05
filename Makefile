SRCS=$(wildcard *.c)

PROGS=$(patsubst %.c,%,$(SRCS))

all: $(PROGS)

%: %.c
	gcc -o $@ $<

clean: 
	rm -f $(PROGS)
