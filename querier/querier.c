#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "../common/pagedir.h"
#include "../common/index.h"
#include "../common/word.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/counters.h"

/*
querier

Input: a page directory where the crawler stored files (string/char*) and a file to read an index created by the indexer (string/char*)
Output: Writes to stdout a sorted list of doc IDs and their URLs from the page directory that best match a query given by the user

Use: When run, the querier will ask for a query from the user. This query can consist of various words and 'and' and 'or' operators. 
The querier will then use those words to search the index for matching documents and present them to the user based on score. The score is 
calculated based on the appearance of the words you search in conjunction with how they relate to the 'and' and 'or' operators you give


Jasper Tucker, May 2023

EXIT CODES:
exit code 0: program ran as normal
exit code 1: argument error
exit code 2: could not allocate memory
exit code 3: problem with given query
*/

// struct to hold all counters. Used for 'and' operator
typedef struct allCounters {
    counters_t* docList1;
    counters_t* docList2;
    counters_t* docsResult;
    } allCounters_t;

// struct to hold the current max score with its corresponding id, as well as a sorted set by score
typedef struct maxData {
    int id;
    int score;
    counters_t* sortedResults;
    } maxData_t;

int fileno(FILE *stream);

static void checkArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename);
static void getQuery(char** query);
static char** cleanQuery(char* query, int* wordCount);
static void checkQuery(char** words, int wordCount);
static void getDocuments(char** words, int wordCount, index_t* index, counters_t** docsResult);
static void orOperator(counters_t** docList1, counters_t** docList2, counters_t** docsResult);
static void addDocuments(void* docsResult, const int key, const int score);
static void andOperator(counters_t** docList1, counters_t** docList2, counters_t** docsResult);
static void compareDocuments(void* allCounters, const int key, const int score);
static void printResults(counters_t** docsResult, char** pageDirectory);
static void printDocument(void* pageDirectory, const int id, const int score);
static void findMaxScore(counters_t** docsResult, counters_t** sortedResults);
static void getCountersSize(void* size, const int id, const int score);
static void compareScores(void* maxData, const int id, const int score);

int main(int argc, char* argv[])
{
    char* pageDirectory = NULL;
    char* indexFilename = NULL;
    int wordCount = 0;

    checkArgs(argc, argv, &pageDirectory, &indexFilename);

    index_t* index = read_index(indexFilename);
    char* query = malloc(32);
    getQuery(&query);

    char** words = cleanQuery(query, &wordCount);

    free(query);

    checkQuery(words, wordCount);

    counters_t* docsResult = counters_new();
    getDocuments(words, wordCount, index, &docsResult);
    counters_t* sortedResults = counters_new();
    findMaxScore(&docsResult, &sortedResults);

    printResults(&sortedResults, &pageDirectory);

    counters_delete(docsResult);
    counters_delete(sortedResults);
    index_delete(index);

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
    else if (argc == 2){
        fprintf(stderr, "Too few arguments were given - pass a page directory where the crawler stored files and a file to write the index to\n");
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

        // check that argv[2] is a valid file to read index from
        FILE* indexFile = fopen(argv[2], "r");
        if (indexFile == NULL) {
            fprintf(stderr, "file to get index from (second arg) is not a valid file with read permissions\n");
            exit(1);
            
        } else {

            // good to go, assign indexFilename to argv[2]
            *indexFilename = argv[2];
            fclose(indexFile);
        }
    }
}

// prompts the user for a query and puts it into a char*
static void getQuery(char** query)
{
    char c;
    size_t inputLength = 0, bufferSize = 0;

    if (isatty(fileno(stdin)))
        printf("Query?: ");

    while ((c = getchar()) != EOF) {

        if (isalpha(c) == 0 && c != '\n' && c != ' ') {
            fprintf(stderr, "Numbers or special characters are not allowed\n");
            exit(1);
        }

        // Allocate memory for the buffer dynamically
        if (inputLength >= bufferSize) {
            bufferSize += 32;  // Increase buffer size by 32 bytes
            *query = realloc(*query, bufferSize);
            if (!*query) {
                fprintf(stderr, "not enough memory to store input from user\n");
                exit(2);
            }
        }

        // Store the character in the buffer
        (*query)[inputLength++] = c;
    }

    // Terminate the buffer with a null character
    (*query)[inputLength] = '\0';
}

// gets a list of char* words from the query to look for documents
static char** cleanQuery(char* query, int* wordCount)
{
    char** words = malloc(sizeof(char*) * strlen(query));
    if (words == NULL) {
        fprintf(stderr, "did not have memory to allocate for words\n");
            exit(1);
    }

    int numWords = 0;
    int start = 0;

    for (int i = 0; i <= strlen(query); i++) {

        // when we encounter a space (or end)...
        if (isspace(query[i]) || i == strlen(query)) {

            int wordLength = i - start;

            if (wordLength > 0) {
                
                // allocate space for a new word, and write it in
                words[numWords] = malloc(sizeof(char) * (wordLength + 1));
                strncpy(words[numWords], &query[start], wordLength);

                // add null character
                words[numWords][wordLength] = '\0';

                // normalize
                words[numWords] = normalize_word(words[numWords]);

                // next word will go into next spot in array
                numWords++;

            }

            // get start of word to be after last one
            start = i + 1;
        }
    }

    // store number of words for later use
    *wordCount = numWords;

    printf("Clean Query: ");

    // print query
    for (int i = 0; i < numWords; i++) {
        printf("%s ", words[i]);
    }

    printf("\n");

    return words;
    
}

// check that the query has operators in the correct places
static void checkQuery(char** words, int wordCount)
{
    // check first word
    if (strcmp(words[0], "and") == 0 || strcmp(words[0], "or") == 0) {
        fprintf(stderr, "an operator cannot be your first word, you used '%s'\n", words[0]);
        exit(3);
    }
  
  //check if operators are next to each other
  if (wordCount > 1) {
    for (int i = 0; i < wordCount - 1; i++) {

        if (strcmp(words[i], "and") == 0 || strcmp(words[i], "or") == 0) {
            if (strcmp(words[i + 1], "and") == 0 || strcmp(words[i + 1], "or") == 0) {

                fprintf(stderr, "operators cannot be next to each other, you used '%s' and '%s'\n", words[i], words[i + 1]);
                exit(3);
            }
        }
    }
  
        // check last word
        if (strcmp(words[wordCount - 1], "and") == 0 || strcmp(words[wordCount - 1], "or") == 0) {
            fprintf(stderr, "an operator cannot be your last word, you used '%s'\n\n", words[wordCount - 1]);
            exit(3);
        }
  }

}

// merges two sets if we want one 'or' the other
static void orOperator(counters_t** docList1, counters_t** docList2, counters_t** docsResult)
{
    // add each pair in docList1 to docsResult
    counters_iterate(*docList1, *docsResult, addDocuments);

    // add each pair in docList2 to docsResult
    counters_iterate(*docList2, *docsResult, addDocuments);
}

// adds an (id, score) pair into a set, updating the score if already found
static void addDocuments(void* docsResult, const int key, const int score)
{
    // if document is already in set, update its score
    if (counters_get(docsResult, key) != 0) {
        for (int i = 0; i < score; i++)
            counters_add(docsResult, key);
    } else {
        // if not in set, create a counter structure in docsResult with the key and score from counters set this is called on
        counters_set(docsResult, key, score);
    }

}

// modifies a set that includes docs from one set 'and' another
static void andOperator(counters_t** docList1, counters_t** docList2, counters_t** docsResult)
{
    allCounters_t allCounters = {
        *docList1,
        *docList2,
        *docsResult
    };

    // if we have no sum, use the first word
    int size = 0;
    counters_iterate(*docsResult, &size, getCountersSize);

    if (size == 0)
        counters_iterate(*docList1, &allCounters, compareDocuments);
    else
        counters_iterate(*docsResult, &allCounters, compareDocuments);
}

// checks if document exists within two sets, and if so, adds it to a new set with a combined score
static void compareDocuments(void* allCounters, const int key, const int score)
{
    allCounters_t* counters = (allCounters_t*)allCounters;
    
    // for each doc in this set, is it also in the new word set?
    int score2 = counters_get((counters_t*)counters->docList2, key);
    int lowerScore = 0;

    // get smaller score
    if (score < score2)
        lowerScore = score;
    else
        lowerScore = score2;

    // if this doc exists in both sets...
    if (score != 0 && score2 != 0) {
        // set this doc in docsResult to lower score of the two sets
        counters_set((counters_t*)counters->docsResult, key, lowerScore);
    } else {
        // if not, remove it (set it to 0)
        counters_set((counters_t*)counters->docsResult, key, 0);
    }
}

// given an index and a list of char* words, uses the words to determine which documents are relevant and what their score is
static void getDocuments(char** words, int wordCount, index_t* index, counters_t** docsResult)
{
    
    // iterate through all words
    for (int i = 0; i < wordCount; i++)
    {
        counters_t* docList1 = index_find(index, words[i]);
        counters_t* docList2 = NULL;
    
        if (strcmp(words[i], "and") != 0 && strcmp(words[i], "or") != 0) {

            if (i + 2 < wordCount)
                docList2 = index_find(index, words[i + 2]);

            if (wordCount == 1){
                //if we only have one word, union it with a null set (use only the set concerning that one word)
                orOperator(&docList1, &docList2, docsResult);
            }
            else {

                if (i + 2 < wordCount && strcmp(words[i + 1], "and") == 0){
                    andOperator(&docList1, &docList2, docsResult);
                }
                else if (i + 2 < wordCount && strcmp(words[i + 1], "or") == 0){
                    orOperator(&docList1, &docList2, docsResult);
                }
                else if (i != wordCount - 1) {

                    // if no operator is explicitly given, assume it is an 'and' operator, unless this is the last word
                    docList2 = index_find(index, words[i + 1]);
                    andOperator(&docList1, &docList2, docsResult);
                }
            }
        }
    }

    // Free list of words
    for (int j = 0; j < wordCount; j++)
        free(words[j]);

    free(words);
}

// find the highest score and add it to a sorted set for each pair in the results
static void findMaxScore(counters_t** docsResult, counters_t** sortedResults)
{
    // get size of DocsResult
    int size = 0;
    counters_iterate(*docsResult, &size, getCountersSize);

    if (size == 0)
        printf("No documents match.\n");

    maxData_t maxData = {
            0,
            0,
            *sortedResults
        };

    maxData_t* data = &maxData;

    // for the total size of docsResult...
    for (int i = 0; i < size; i++) {

        // find highest score
        counters_iterate(*docsResult, data, compareScores);
        // add that to a new sorted set
        counters_set(data->sortedResults, data->id, data->score);
        // set the old score to 0 so it isn't considered again
        counters_set(*docsResult, data->id, 0);
        // reset max score for next iteration
        data->score = 0;
        data->id = 0;
    }
}

// increment a counter for each element
static void getCountersSize(void* size, const int id, const int score)
{
    int* s = (int*)size;
    
    if (score > 0)
        *s += 1;
}

// finds the higher of two scores, and stores data about that document with the score
static void compareScores(void* maxData, const int id, const int score)
{
    maxData_t* data = (maxData_t*)maxData;

    if (score > data->score)
    {
        data->score = score;
        data->id = id;
    }  
}

// prints each document in the resulting counters set
static void printResults(counters_t** docsResult, char** pageDirectory)
{
    counters_iterate(*docsResult, (void*)*pageDirectory, printDocument);
}

// prints one document's score, id, and URL
static void printDocument(void* pageDirectory, const int id, const int score)
{
    // get URL by creating a webpage from the page path
    webpage_t* webpage = pagedir_load((char*)pageDirectory, id);
    char* URL = webpage_getURL(webpage);

    // disregard ones we "deleted" earlier
    if (score != 0)
        printf("Score: %d, Doc ID: %d, URL: %s\n", score, id, URL);

    webpage_delete(webpage);
}
