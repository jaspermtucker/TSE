# CS50 TSE Querier
## Design Spec

The TSE *querier* is a program that takes a query from the user and displays a list of documents, ranked by relevance, which relate to the user's query.

### User interface

The querier's only interface with the user is on the command-line; it must always have two arguments.

```
querier pageDirectory indexFilename
```

For example, if `letters` is a pageDirectory in `../data`,

``` bash
$ querier ../data/letters ../data/letters.index
```

### Inputs and outputs

**Input**: the querier reads files from a directory by constructing file pathnames from the `pageDirectory` parameter followed by a numeric document ID (as described in the Requirements).

The querier reads document files by searching for their ID

The querier also reads an `index` created from `indexFilename` to get informtion about the pages, namely what words are used in them and how frequently

**Output**: We write to stdout a list of documents to the user that match their query. The format is Score, Doc ID, URL. Score relates to the frequency of words appearing.

### Functional decomposition into modules

We anticipate the following modules or functions:

 1. *main*, which parses arguments and initializes other modules;
 2. *getQuery*, which gets a query from the user;
 2. *cleanQuery*, which makes sure the query is readable by the program;
 3. *checkQuery*, which makes sure there are no invalid use of operators in the query;
 4. *getDocuments*, which gets the documents that match the query;
 5. *orOperator*, which handles if the user types 'or';
 6. *andOperator*, which handles if the user types 'and';
 7. *printResults*, which prints the documents to the user;
 8. *findMaxScore*, which ranks the documents by their score;

And some helper modules that provide data structures:

 1. *index*, a module providing the data structure to represent the in-memory index, and functions to read and write index files;
 1. *webpage*, a module providing the data structure to represent webpages, and to scan a webpage for words;
 2. *pagedir*, a module providing functions to load webpages from files in the pageDirectory;
 4. *word*, a module providing a function to normalize a word.

### Pseudo code for logic/algorithmic flow

The querier will run as follows:

    parse the command line, validate parameters, initialize other modules, get and clean the query, get the appropriate documents, rank the documents, and print them to the user.

Psuedo code for all functions can be found in IMPLEMENTATION.md

### Major data structures

The key data structures are *index*, mapping from *word* to *(docID, #occurrences)* pairs, *allCounters*, storing *counters* data structures for two document lists, as well as the running sum of documents to present to the user, and *maxData*, which stores the id and score of the current highest scoring document, as well as a sorted *counters* of the highest ranked documents.
The *index* is a *hashtable* keyed by *word* and storing *counters* as items.
The *counters* is keyed by *docID* and stores a count of the number of occurrences of that word in the document with that ID. 

### Testing plan

*testing.sh* will serve as the testing mechanism for *querier*, using both *fuzzquery* and valgrind. With it, it will test bad arguments to make sure errors work correctly, good arguments to ensure it gives expected output, and runs with valgrind to check for memory leaks.

1. Test `querier` with various invalid arguments.
	2. no arguments
	3. too many arguments
	5. invalid `pageDirectory` (non-existent path)
	5. invalid `pageDirectory` (not a crawler directory)
	7. invalid `indexFile` (read-only directory)
8. Run *querier* with valid arguments, and querys that use a mix of 'or', 'and', and implied 'ands' (when two words are next to each other)
9. Run *valgrind* on *querier* to ensure no memory leaks or errors.