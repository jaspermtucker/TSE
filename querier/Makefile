# Makefile for querier
#
# Jasper Tucker, May 2023

C = ../common
L = ../libcs50

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L -I$C
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all
LLIBS = $C/common.a $L/libcs50-given.a

.PHONY: all test valgrind clean

all: querier fuzzquery

querier: querier.o $(LLIBS)
	$(CC) $(CFLAGS) $^ -o querier

querier.o:
	$(CC) $(CFLAGS) $^ -c querier.c

fuzzquery: fuzzquery.o $(LLIBS)
	$(CC) $(CFLAGS) $^ -o fuzzquery

fuzzquery.o:
	$(CC) $(CFLAGS) $^ -c fuzzquery.c	

test: querier testing.sh
	bash -v testing.sh

# short example test of querier using valgrind, expects querier to be made
valgrind: querier
	$(VALGRIND) ./querier ~/cs50-dev/shared/tse/output/toscrape-2 ~/cs50-dev/shared/tse/output/toscrape-2.index

clean:
	rm -f querier querier.o fuzzquery fuzzquery.o
	rm -rf ../data