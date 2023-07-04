# Makefile for common.a library
#
# Jasper Tucker, April 2023

L = ../libcs50

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all
LLIBS = $L/libcs50.a

common.a: pagedir.o index.o word.o
	ar cr common.a pagedir.o index.o word.o

pagedir.o: pagedir.h pagedir.c
	$(CC) $(CFLAGS) -c pagedir.c

index.o: index.h index.c
	$(CC) $(CFLAGS) -c index.c

word.o: word.h word.c
	$(CC) $(CFLAGS) -c word.c

.PHONY: clean

clean:
	rm -f pagedir.o index.o word.o common.a


