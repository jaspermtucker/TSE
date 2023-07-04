#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common/index.h"

/*
indextest

Input: a filepath to a file produced by the indexer (string/char*) and another filepath to a file where a copy can be written (string/char*)
Output: Writes to a file (the second argument you give) an equivalent version of the index contents in a file made by the indexer (the first argument you give)

Use: The indextest is a way to check that saving and writing an index into files works well. It takes the contents of a file produced by the indexer,
creates a new index object storing its data, and then writes that new data to a different file. The majority of the work done occurs in index.c;
this file serves mostly to check arguments and organize function calls.

Jasper Tucker, April - May 2023

EXIT CODES:
exit code 0: program ran as normal
exit code 1: argument error
*/

static void checkArgs(const int argc, char* argv[], char** oldIndexFilename, char** newIndexFilename);

int main(int argc, char* argv[])
{
    char* oldIndexFilename = NULL;
    char* newIndexFilename = NULL;

    checkArgs(argc, argv, &oldIndexFilename, &newIndexFilename);

    // open file for writing, checked for null in checkArgs
    FILE* file = fopen(newIndexFilename, "w");

    // create index and write it into another file
    write_index(read_index(oldIndexFilename), file);

    printf("done copying index\n");
    return 0;
}

// check that each argument is correct and write them to their corresponding variables
static void checkArgs(const int argc, char* argv[], char** oldIndexFilename, char** newIndexFilename)
{
     if (argc == 1) {
        fprintf(stderr, "No arguments were given - pass an index file created by the indexer and a new file to write a copy of it to\n");
        exit(1);
    }
    else if (argc > 3){
         fprintf(stderr, "Too many arguments were given - pass an index file created by the indexer and a new file to write a copy of it to\n");
         exit(1);
    }
    else {
         FILE* oldIndexFile = fopen(argv[1], "r");
        if (oldIndexFile == NULL) {

            fprintf(stderr, "file to read index from (first arg) is not a valid file with read permissions\n");
            exit(1);

        } else {

            // good to go, assign oldIndexFilename to argv[1]
            *oldIndexFilename = argv[1];
            fclose(oldIndexFile);
        }

         FILE* newIndexFile = fopen(argv[2], "w");
        if (newIndexFile == NULL) {

            fprintf(stderr, "file to write new index to (second arg) is not a valid file with write permissions\n");
            exit(1);

        } else {

            // good to go, assign newIndexFilename to argv[3]
            *newIndexFilename = argv[2];
            fclose(newIndexFile);
        }
    }
}

