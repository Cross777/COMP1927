# COMP1927 14s2 - Assignment 1 Makefile

CC=gcc
CFLAGS=-Wall -Werror -gdwarf-2
# remove -gdwarf-2 when submitting

vlad : vlad.o allocator.o

vlad.o : vlad.c allocator.h

allocator.o : allocator.c allocator.h

clean :
	rm -f vlad *.o
