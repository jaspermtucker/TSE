# CS50 Lab 5
## CS50 Spring 2023

### querier

The `querier` takes a directory of pages built by the `crawler`, and an `index` built by the `indexer`. From those, it takes a query from the user and prints to stdout a ranked list of the relevant webpages.

### Implementation

The `querier` begins by parsing the command line and validating parameters. It then prompts the user for a query and stores that as a `char*`. It then cleans the query and makes sure it is valid, and then stores the separate words in the query as a `char**`. Looping through all the words, it then finds the documents that match that word. During this process, it takes the 'and' and 'or' keywords into considering, either adding or omitting certain entries depending on what is written. The results are then ranked by a `score`. The `score` is the number of occurrences of a word in a document, and each document has its own `score`. The `score` is influenced by the 'and' and 'or' operators (specfics on how can be found in the requirements). At the end, the `querier` prints the ranked documents to the user.

More implementation is discussed in IMPLEMENTATION.md

### Assumptions

There are not many assumptions outside of those outlined in the directions for the assignment, or cleared in conversations with TAs.

The only assumption I make is in what a 'clean query' is. For this program, a clean query is a full valid query, with each word seperated by one space, all lowercase. 

### Compilation and Testing

`make` can be run to compile all needed files. `make test` can be run to test the querier after `make` has been run. `make valgrind` will test a short example with valgrind, it requires both `make` to be run. `make clean` can be used to delete all the files created by `make` or `make test`.