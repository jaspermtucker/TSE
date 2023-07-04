#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common/pagedir.h"
#include "../common/index.h"
#include "../common/word.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/webpage.h"
#include "../libcs50/counters.h"

/*
indexer

Input: a page directory where the crawler stored files (string/char*) and a file to write the index to (string/char*)
Output: Writes to a file (the second argument you give) an 'index' created using the files within the crawler produced directory (first argument)

Use: The indexer looks at each file within the crawler produced page directory. For each page, it grabs each instance of a word (> 3 letters) and counts the 
occurences of that word in the page. It then writes to an index file a list of the words it finds, with a corresponding list of the document IDs it found them in,
as well as the number of occurences for each one. The format is [word] [id] [count] [id] [count]...


Jasper Tucker, April - May 2023

EXIT CODES:
exit code 0: program ran as normal
exit code 1: argument error
exit code 2: could not allocate memory
exit code 3: error opening file
*/

static index_t* indexBuild(const char* pageDirectory);
static void indexPage(webpage_t* webpage, int docID, index_t* index);

static void checkArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename);

int main(int argc, char* argv[])
{
    char* pageDirectory = NULL;
    char* indexFilename = NULL;

    checkArgs(argc, argv, &pageDirectory, &indexFilename);

    // open file for writing, checked for null in checkArgs
    FILE* file = fopen(indexFilename, "w");

    // build index and write to indexFilename
    write_index(indexBuild(pageDirectory), file);

    printf("finished\n");
    // file is closed and index is deleted in write_index
    return 0;
}

// check that each argument is correct and write them to their corresponding variables
static void checkArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename)
{
    // give error if incorrect number of arguments
    if (argc == 1) {
        fprintf(stderr, "No arguments were given - pass a page directory where the crawler stored files and a file to write the index to\n");
        exit(1);
    }
    else if (argc > 3){
         fprintf(stderr, "Too many arguments were given - pass a page directory where the crawler stored files and a file to write the index to\n");
         exit(1);
    }
    else {

        // check that argv[1] is a valid page directory
        if (!pagedir_validate(argv[1]))
        {
            fprintf(stderr, "page directory passed (first arg) is not a valid, crawler produced, directory\n");
            exit(1);
        }

        // good to go, assign pageDirectory to argv[1]
        *pageDirectory = argv[1];

        // check that argv[2] is a valid file to put index, and if not, create it
        FILE* indexFile = fopen(argv[2], "w");
        if (indexFile == NULL) {
            fprintf(stderr, "file to save index to (second arg) is not a valid file with write permissions\n");
            exit(1);
            
        } else {

            // good to go, assign indexFilename to argv[2]
            *indexFilename = argv[2];
            fclose(indexFile);
        }

        printf("arguments checked\n");
    }
}

// creates and calls methods to fill an 'index' object that stores data from the page directory
static index_t* indexBuild(const char* pageDirectory)
{
    printf("building index\n");
    // create "index"
    index_t* index = index_new(500);
    int id = 1;

    // for all pages you can find, create a webpage and pass to indexPage
    webpage_t* webpage = NULL;
    while ((webpage = pagedir_load(pageDirectory, id)) != NULL) {
        indexPage(webpage, id, index);
        webpage_delete(webpage);
        id++;
    }

    return index;
}

// does the work of searching through words in a page and adding them and their corresponding
// data to the 'index' object
static void indexPage(webpage_t* webpage, int docID, index_t* index)
{
    char* word = NULL;
    int pos = 0;
    // for each word in webpage...
    while ((word = webpage_getNextWord(webpage, &pos)) != NULL) {

        if (strlen(word) >= 3) {
            // convert word to lowercase
            word = normalize_word(word);

            // look for word in index
            counters_t* counters = NULL;
            if ((counters = index_find(index, word)) != NULL) {
                // if word found, increment count related to docID
                counters_add(counters, docID);
            } else {
                // create counters for docID and add to index if word is not found
                counters = counters_new();
                counters_set(counters, docID, 1);

                index_insert(index, word, counters);
            }
        }

        free(word);
     }
}
