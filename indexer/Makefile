# Makefile for indexer
#
# Jasper Tucker, April/May 2023

C = ../common
L = ../libcs50

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L -I$C
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all
LLIBS = $C/common.a $L/libcs50-given.a

.PHONY: all test valgrind clean

all: indexer indextest

indexer: indexer.o $(LLIBS)
	$(CC) $(CFLAGS) $^ -o indexer

indexer.o:
	$(CC) $(CFLAGS) $^ -c indexer.c

indextest: indextest.o $(LLIBS)
	$(CC) $(CFLAGS) $^ -o indextest

indextest.o:
	$(CC) $(CFLAGS) $^ -c indextest.c

test: indexer indextest testing.sh
	bash -v testing.sh

# short example test of indexer using valgrind, expects output from the crawler valgrind test (requires directory ../data/letters)
valgrind: indexer
	touch ../data/lettersIndex
	chmod +w ../data/lettersIndex
	$(VALGRIND) ./indexer ../data/letters ../data/lettersIndex

clean:
	rm -f indexer indexer.o indextest indextest.o
	rm -rf ../data