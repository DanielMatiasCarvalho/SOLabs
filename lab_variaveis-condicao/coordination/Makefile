# Makefile
#
# It uses implicit rules to compile .c files using the
# values assigned to macros CC and CFLAGS
#

CC = gcc
CFLAGS = -g -Wall -std=c17 -pthread
LDFLAGS = -pthread

all: coordination coordination_condvar

coordination: coordination.o

coordination_condvar: coordination_condvar.o

fmt:  coordination.c coordination_condvar.c
	clang-format -i $^

clean:
	rm -f *.o coordination coordination_condvar
