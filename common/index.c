#include <stdio.h>
#include <stdlib.h>
#include "counters.h"
#include "hashtable.h"
#include "file.h"

/*
index

Use: A module providing the data structure to represent the in-memory index, and functions to read and write index files

Jasper Tucker, April - May 2023
*/

typedef hashtable_t index_t;

static void writefunc(void* file, const char* word, void* counters);
static void writecounters(void* file, const int docID, const int count);
static void deletecounters(void* counters);

// creates a new index
index_t* index_new(const int num_slots)
{
  return (index_t*)hashtable_new(num_slots);
}

// inserts a new (key, value) pair into the index
// a value is a counters object (docID, count)
void index_insert(index_t* index, char* word, counters_t* counters)
{
  hashtable_insert(index, word, counters);
}

// returns a counters object (docID, count) given a word
counters_t* index_find(index_t* index, char* word)
{
  return (counters_t*)(hashtable_find(index, word));
}

// deletes the index
void index_delete(index_t* index)
{
  hashtable_delete((hashtable_t*)index, deletecounters);
}

// writes the index to a file, given the index and file
void write_index(index_t* index, FILE* file)
{
  // call writing function on each element in index
  // largely works as follows:

    // for all words in index
        // print word (space)
        // for all counters in word
            // print id (space) count (space)
        // print \n

  if (index != NULL && file != NULL) {

    printf("writing index to file\n");

    // write each element in hashtable
    hashtable_iterate(index, file, writefunc);

    fclose(file);
    index_delete(index);
  }
}

// creates an index from a given file, and returns it
index_t* read_index(char* fileName)
{
  index_t* index = index_new(500);

  // get file to read
  FILE* file = fopen(fileName, "r");
  if (file == NULL) {

    fprintf(stderr, "file to read index from file\n");
    return NULL;

  }

  char* word = NULL;
  counters_t* counters = NULL;
  int id = 0, count = 0;

  // while we find words in file...
  while ((word = file_readWord(file)) != NULL) {

    counters = counters_new();
    if (counters == NULL){
      fprintf(stderr, "not enough memory to create webpage\n");
      return NULL;
    }

    // update counter objects with found ids and counts
    while ((fscanf(file, " %d %d", &id, &count) == 2)) 
      counters_set(counters, id, count);
  
    // put word into index
    index_insert(index, word, counters);

    free(word);
  }

  fclose(file);
  return index;
}

// a helper function to write the keys (words) of the hashtable,
// and then iterate through each counters object to write them
static void writefunc(void* file, const char* word, void* counters)
{
  // print word to file
  fprintf((FILE*)file, "%s ", word);

  // call other writing function to handle counters
  counters_iterate((counters_t*)counters, file, writecounters);

  // print new line for next word
  fprintf((FILE*)file, "\n");
}

// a helper function to write the keys (docID) and items (count)
// within each counters
static void writecounters(void* file, const int docID, const int count)
{
  // print id and count for counters
  fprintf((FILE*)file, "%d %d ", docID, count);
}

// a helper function to delete each counters object
static void deletecounters(void* counters)
{
  counters_delete((counters_t*)counters);
}