# Makefile
#
# It uses implicit rules to compile .c files using the
# values assigned to macros CC and CFLAGS
#

CC = gcc
CFLAGS = -g -Wall -std=c17 -fsanitize=thread
TARGETS = thread

# We need to pass into a library flag -lpthread. 
# -l<name> tries to find a lib<name>.so (a shared object, or, in other words, a compiled library) 
# file and link it in the executable
#
# The variable LDFLAGS gets used the implicit rules
LDFLAGS = -pthread

.PHONY: all clean

all: $(TARGETS)

clean:
	rm -f *.o $(TARGETS) 
