# CS50 TSE Querier
## Implementation Spec

In this document we focus on the implementation-specific decisions. For a discussion about the design of the querier, see DESIGN.md
The various sections go as follows:

-  Data structures
-  Control flow: pseudo code for each of the functions. A broader pseudo code for the overall flow can be found in DESIGN.md
-  Detailed function prototypes and their parameters
-  Error handling and recovery
-  Testing plan

## Data structures 

The main structures in use are the `index` and `counters`. An `index` is a `hashtable` that maps from `char*` keys to `counters` items. `counters` are a collection of (key, item) pairs where a given key will return the item. The item in this case is a count associated with a key that has been added to `counters`. We also create two more structs, `allCounters` and `maxData`. `allCounters` is used to store the `counters` for two different words, as well as a sum `counters`. It is helpful for when a function can only take one argument, but needs access to multiple `counters`. `maxData` stores an int for a document id and score, as well as a `counters` for the final sorted set. It is helpful for the same reason as `allCounters`.

## Control flow

The Querier is implemented in one file `querier.c`, with fifteen functions, and recruits functions from other modules, most promnently: `pagedir.c`, `index.c`, `word.c`, and `counters.c`.

### main

The `main` function calls various functions to get a query from the user and present the relevant information to them.

### checkArgs

Given arguments from the command line, validate that they can be used without error.

If not exactly two arguments are given, the program exits with an error.

* for `pageDirectory`, call `pagedir_validate()` from `pagedir.c`
* for `indexFilename`, open the file for reading
* if any trouble is found, print an error to stderr and exit non-zero.

### getQuery

Prompts the user for a query and places it into a `char*` variable, given as a `char**` parameter. If the user does not give a valid query, exit with an error.

Pseudocode:

	prompt the user for input
	while there is another character in ther input
		if it is not a letter or space
            exit with an error
        if we need more memory for the char* variable to save query to
            realloc more memory
        add the character into the variable
    add the null-terminating character into the variable

### cleanQuery

Given the query, turns it into a usable, normalized set of words to use to later search the `index`.

Pseudocode:

	allocate memory for the words of the query
    for each character in the query
        if the character is a space
            calculate how long the word before the space is
            allocate memory for that word
            copy that section of the query into the word
            add a null-terminating character
            normalize the word
    print out each word to the user

### checkQuery

Checks that there are not any problems with the order of operators (and / or) in the query.

Pseudocode:

    given the words in the query
	if the first word is an operator
        exit with an error
    if there are more than 1 word
        if there are two operators next to each other
            exit with an error
        if the last word is an operator
            exit with an error

### orOperator

Does the 'or' operation if the user uses that operator in their query. Makes use of the `addDocuments` function mentioned later.

Pseudocode:

    add each counters structure from the first word to a sum set
    add each counters structure from the second word to a sum set

### addDocuments

Adds a counters pair into a sum set. Used by the `orOperator` function.

Pseudocode:

    if the key for the counters pair is already in the sum set
        increment its value by the new value
    else
        add the pair into the set

### andOperator

Does the 'and' operation if the user uses that operator in their query. Makes use of the `compareDocuments` function mentioned later.

Pseudocode:

    create an allCounters to hold the two sets we want to compare and the sum set
    compare each element in our sum set (or first set if there is no current sum) and the second set

### compareDocuments

Checks if two documents (keys within counters) exists within two sets, and if so, adds them to a sum set. It also sets the value of the added pair to the lower item value of the two.

Pseudocode:

    get the item value of each key (document id) in the second set
    store the lower item of the two
    if the key is present in each set
        add a pair to the sum set with that key and the lower item
    else
        remove it from the sum set

### getDocuments

Determines which `counters` (document id, score) pairs are relevant, given the words in the query that we got from the user.

Pseudocode:

    for each word
        get the list of counters
        if the word is not an operator
            get the list of counters for the word two in front of this one
            if this is the only word
                put this word's counters in the results
            else
                if the next word is 'and'
                    call andOperator
                if the next word is 'or'
                    call orOperator
                else if this is not the final word
                    call andOperator

    free the words

### findMaxScore

Sorts the results of our documents by score into a new sorted set by score.

Pseudocode:

    get the size of our results
    create a maxData structure
    for the size of the results
        find the current max score
        add the document with the max score to a sorted set
        set that score and id to 0

### getCountersSize

Gets the size of the counters structure.

Pseudocode:

    if the score is > 0
        increment the size by 1

### compareScores

Finds the higher of two scores and stores it.

Pseudocode:

    if the score is higher than the current max
        set the current max to this score
        set the associated id to this id

### printResults

Calls printDocument on each element of a counters set.

Pseudocode:

    call the iterative printing function.

### printDocument

Prints the score, id, and url of a document

Pseudocode:

    create a webpage structure using pagedir_load
        if the score is not 0
    print the score, id, and url of the page
    delete the webpage

## Other modules

### pagedir

This module handles the `pagedir_validate()` and `pagedir_load()` methods used by the `querier`.

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

This module handles the `read_index()` function used by `querier`. It also contains methods for creating, deleting, inserting, or finding elements in an `index`, but these are merely wrappers for the `hashtable` module.

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

### querier

```c
int main(int argc, char* argv[]);
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
```



### pagedir

These first two methods are implemented in `pagedir.c`, but are not used by the `querier`.
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

The `querier` is tested using `testing.sh`, which calls the `querier` a number of times both with good and bad arguments. It tests that the `querier` is able to handle strange cases and throw the necessary errors, as well as function properly with regular arguments. The scoring can be checked by observing if it is the correct order. `fuzzquery` is also used to test `querier` with other odd cases. `valgrind` is also used to check for memory leaks.

### Specifics on integration testing

Run `testing.sh` with `bash -v testing.sh` so the output of `querier` is intermixed with the commands used to invoke it.
Verify correct behavior by studying the output.