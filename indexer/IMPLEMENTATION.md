# CS50 TSE Indexer
## Implementation Spec

In this document we focus on the implementation-specific decisions.
The various sections go as follows:

-  Data structures
-  Control flow: pseudo code for overall flow, and for each of the functions
-  Detailed function prototypes and their parameters
-  Error handling and recovery
-  Testing plan

## Data structures 

The main structure in use is an `index`. An `index` is a `hashtable` that maps from `char*` keys to `counters` items. 

## Control flow

The Indexer is implemented in one file `indexer.c`, with four functions, and recruits functions from other modules, most promnently: `pagedir.c`, `index.c`, and `word.c`.

### main

The `main` function calls various functions to create the index. It first calls `checkArgs`, then opens a file to write to, and calls `indexBuild` within `write_index`. At the end it exits 0.

### parseArgs

Given arguments from the command line, extract them into the function parameters; return only if successful.

If not exactly two arguments are given, the program exists with an error.

* for `pageDirectory`, call `pagedir_validate()` from `pagedir.c`
* for `indexFilename`, open the file for writing
* if any trouble is found, print an error to stderr and exit non-zero.

### indexBuild

Creates the index data structure and organizes function calls to create it. This makes use of `pagedir_load()` from `pagedir.c` and `indexPage`. Both are discussed later on in more detail

Pseudocode:

	initialize the index
	while there is another webpage that can be created from a page in the page directory
		add that page into the index
		delete the webpage
	return the index

### indexPage

Searches through words in a webpage and adds them to the index. This makes use of the `normalize_word()` function from the `word` module.

Pseudocode:

	while there is another word in the page
		if that word is >= 3 characters
			normalize the word
			if there is a counters structure for this word in the index
				increment its count
			else
                create a counters structure for this word and insert in into the index with a count of 1
	    free the word

## Other modules

### pagedir

This module handles the `pagedir_load()` and `pagedir_validate()` methods used by the `indexer`.

Pseudocode for `pagedir_validate()`:

	construct the pathname for the .crawler file in that directory
	attempt to access that file
	if able to access
        return true
    else
        return false


Pseudocode for `pagedir_load()`:

	construct the pathname for the page file in pageDirectory
	open that file for reading
	read URL into variable
	read depth into variable
	read html into variable
    close the fle
	create a webpage with the URL, depth, and html
    return the webpage

### index

This module handles the `write_index()` method used by the `indexer`, as well as `read_index()`, which is used later by `indextest` for testing. It also contains methods for creating, deleting, inserting, or finding elements in an `index`, but these are merely wrappers for the `hashtable` module.

Pseudocode for `write_index()`:

	for all words in the index
	    print that word to the file with a space
            for all counters for that word
                print the id and count of each element in the counters set with spaces to the file
            print a new line to the file
     close the file
    delete the index


Pseudocode for `read_index()`:

	create an index
    open the file for reading
    for each word in the file
        create a counters set
            while there are ids and counts for the word
                add an (id, count) pair to the counters set  
        insert the (word, counters) pair into the index
        free the word
    close the file
    return the index

### word

This module handles the `normalize_word()` method used by the `indexer`.

Pseudocode for `normalize_word()`:

	for each character in the word
        convert the character to lowercase
    return the word

### libcs50

We leverage the modules of libcs50, most notably `hashtable`, `counters`, and `webpage`.
See that directory for module interfaces.
The new `webpage` module allows us to represent pages as `webpage_t` objects, to fetch a page from the Internet, and to scan a (fetched) page for URLs.

## Function prototypes

### indexer

```c
int main(int argc, char* argv[]);
static void indexPage(webpage_t* webpage, int docID, index_t* index);
static void checkArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename);
```

### indextest

This file is discussed more below with testing.

```c
int main(int argc, char* argv[]);
static void checkArgs(const int argc, char* argv[], char** oldIndexFilename, char** newIndexFilename);
```

### pagedir

These first two methods are implemented in `pagedir.c`, but are not used by the `indexer`.
```c
bool pagedir_init(const char* pageDirectory);
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);
bool pagedir_validate(const char* pageDirectory);
webpage_t* pagedir_load(const char* pageDirectory, int docID);
```

### index

```c
index_t* index_new(const int num_slots);
void index_insert(index_t* index, char* word, counters_t* counters);
counters_t* index_find(index_t* index, char* word);
void write_index(index_t* index, FILE* file);
void index_delete(index_t* index);
index_t* read_index(char* fileName);
```

### word

```c
char* normalize_word(char* word);
```

## Error handling and recovery

All the command-line parameters are checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status.

Out-of-memory errors are handled by checking for `NULL`

All code uses defensive-programming tactics to catch and exit, e.g., if a function receives bad parameters.

That said, certain errors are caught and handled internally: for example, `pagedir_validate` returns false if it cannot find access to a `.crawler` file, allowing the Indexer to decide what to do.

## Testing plan

Here is an implementation-specific testing plan.

### Testing with indextest

The `indexer` relies on several parts working together, so `indextest` is built to make sure everything works properly. `indextest` takes a file produced by the `indexer`, and creates another `index` object, which it then writes to another file. We can then check those files for equivalence.

### Specifics on integration testing

The script `testing.sh` invokes the `indexer` several times, with a variety of command-line arguments.
First, a sequence of invocations with erroneous arguments, testing each of the possible mistakes that can be made.
Second, a run as normal, and then with valgrind
Third, a sequence of invocations with erroneous arguments for `indextest`
Run that script with `bash -v testing.sh` so the output of `indexer` is intermixed with the commands used to invoke the `indexer`.
Verify correct behavior by studying the output, and by sampling the files created in the respective directories (created by the test).