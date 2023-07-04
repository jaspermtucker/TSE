# CS50 Lab 5
## CS50 Spring 2023

### indexer

The `indexer` takes a directory of pages built by the `crawler`, and creates an `index` object, which it then writes to a file. That file displays a list of words (one per line) that are 3 or more characters, along with a list of the document IDs the words are found in, and the number of occurences for each document. It appears as follows:
[word] [id] [count] [id] [count]...

### Implementation

The `indexer` first checks each of the two arguments given by the user to ensure they are correct. It makes use of `pagedir.c` that implements a function to check if the page directory passed is one created by the crawler. It does so by constructing a file path to the .crawler file and checking if it has access to it. It then checks if it has writing access to the file passed where the index should be written.

The file to write the index to is then opened, and `indexBuild()` is called to create the index within `write_index()` which writes that index to the opened file. `indexBuld()` creates a new index object, using the functionality built in `index.c`, and then makes use of `pagedir.c` to create `webpage` data structures from all the pages within the page directory, and indexes each of them (discussed more below).

To create a `webpage`, `pagedir.c` uses `pagedir_load`, which creates a file path to a page, given the page directory and document id. It then uses `readLine()` from `file.c` and `sscanf()` to get the elements in the page, and put them into the `webpage` structure.

With a `webpage` created for a given file in the page directory, `indexer` then calls `indexPage()` on it. `indexPage()` begins by looking at each word found in the `webpage`, making use of the `webpage` module, and then searches the created `index` structure to see if that word exists in it as a key. If it does not, then it creates a `counters` data structure with the corresponding document id of the current page as a key, and occurence of 1 as the item, and inserts it into the `index`. If it does find the key, it increments the number of occurences in its counters structure.

Once this has been done for every page the `indexer` can find in the page directory, it writes it to the file using `write_index()` from the `index` module. `write_index()` iterates over each (word, counters) pair, printing the word to file. Within that, it then iterates through each (id, count) pair within the counters item, and prints each corresponding id and count to the file. At the end, it prints a newline before moving on to the next word.

### indextest

The `indextest` takes a file created by the `indexer` and creates a new `index` that it then writes to another file. The new file produced is equivalent to the old file produced by the indexer.

### Implementation

`indextest` first checks each of the two arguments given by the user to ensure they are correct. It first checks the first argument, the file produced by `indexer`, and makes sure it has read access. It then checks if it has writing access to the file passed where the new index should be written.

The file to write a new index to is then opened, and `read_index()` is called to create a new index from the file where the `indexer` wrote its index. It calls this function within `write_index`, which writes the new `index` to a file.

`read_index` is implemented in `index.c`. It creates a new `index` data structure, and opens the given file to be read from. It then uses `file_readWord()` from `file.c` to get each word from the given file, and creates a new `counters` data structure for each one. It then scans for the IDs and counts on that line (in groups of 2) and sets pairs within the `counters` to have those values. At the end, it inserts the (word, counters) pair into the `index`.

`write_index` is implemented in the same way as described in the implementation for `indexer`.

### Assumptions

There are not many assumptions outside of those outlined in the directions for the assignment, or cleared in conversations with TAs.

### Compilation and Testing

`make` can be run to compile all needed files. `make test` can be run to test the indexer after `make` has been run. `make valgrind` will test a short example with valgrind, it requires both `make` to be run, and `make valgrind` from the crawler should have been run. `make clean` can be used to delete all the files created by `make` or `make test`.