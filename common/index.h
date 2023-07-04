#include <stdio.h>
#include <hashtable.h>
#include <counters.h>

/*
index

Use: A module providing the data structure to represent the in-memory index, and functions to read and write index files

Jasper Tucker, April - May 2023
*/

typedef hashtable_t index_t;

/**************** index_new ****************/
/* 
Creates an index

Inputs: the number of slots to have in the index (works identically to a hashtable)
Outputs: the created index
*/
index_t* index_new(const int num_slots);

/**************** index_insert ****************/
/* 
Inserts a new (key, value) pair into the index. A value is a counters object (docID, count)

Inputs: the index to insert to, the word acting as the key, and the counters object for that key (containing (docID, count) pairs)
Outputs: nothing
*/
void index_insert(index_t* index, char* word, counters_t* counters);

/**************** index_find ****************/
/* 
Returns a counters object (docID, count) given a word

Inputs: the index to search, and the word key
Outputs: the counters object containing (docID, count) pairs
*/
counters_t* index_find(index_t* index, char* word);

/**************** write_index ****************/
/* 
Writes the index to a file, given the index and file

Inputs: the index with information to write and the file to write it to
Outputs: nothing
*/
void write_index(index_t* index, FILE* file);

/**************** index_delete ****************/
/* 
Delete the index

Inputs: the index to delete
Outputs: nothing
*/
void index_delete(index_t* index);

/**************** read_index ****************/
/* 
Creates an index from a given file, and returns it

Inputs: the file to read from to create the index
Outputs: the index created from the file
*/
index_t* read_index(char* fileName);
