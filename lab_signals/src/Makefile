CC=gcc
LD=gcc
CFLAGS=-fsanitize=address -std=c17 -g
LDFLAGS=-fsanitize=address

SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:.c=.o)
TARGETS := $(SOURCES:.c=)

.PHONY: all clean fmt

all: $(TARGETS)

intquit: intquit.o

fmt: $(SOURCES)
	clang-format -i $^

clean:
	@rm -f $(OBJECTS) $(TARGETS)
