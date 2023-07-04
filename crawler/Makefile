# Makefile for crawler
#
# Jasper Tucker, April 2023
C = ../common
L = ../libcs50

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L -I$C
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all
LLIBS = $C/common.a $L/libcs50-given.a

crawler: crawler.o $(LLIBS)
	$(CC) $(CFLAGS) $^ -o crawler

crawler.o:
	$(CC) $(CFLAGS) $^ -c crawler.c

.PHONY: test valgrind clean

test: crawler testing.sh
	bash -v testing.sh

# short example test using valgrind
valgrind: crawler
	mkdir ../data
	mkdir ../data/letters
	$(VALGRIND) ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters 10

clean:
	rm -f crawler crawler.o
	rm -rf ../data