# CS50 Lab 4
## CS50 Spring 2023

### crawler

The `crawler` begins with a URL, and "crawls" that URL for any more URLs it finds. It does this recursively, and with each URL that it finds that is internal to the bounds of the `crawler`, it saves the page's contents to dinstinct files within a directory.

### Implementation

The `crawler` begins by parsing each argument passed by the user. The arguments given are a seed URL, a directory to save pages to, and a depth at which to search. The program ensures that there are only those 3 arguments, and that the seed URL is valid and internal, the directory exists, and that the depth is an `int` between 0 and 10.

The `crawler` then makes use of the `hashtable`, `bag`, and `webpage` modules for its main functionality. Starting with the seed URL, the `crawler` creates a hashtable of the URLs encountered so far, and then creates a webpage from that URL which is added to bag of the webpages to scan.

Recursively, the `crawler` then takes a webpage out of the bag, saves its page contents to a file (discussed later), and scans it for more URLs. By scanning the webpage for more URLs, it adds more webpages to the bag (if it finds any), which results in the while loop continuing until it no longer finds URLs, or reaches its max depth. 'Depth' refers to a URL's distance away from the seed URL (with 0 being the seed URL itself, and 1 being URLs on that page...)

To save a page's content, the `crawler` makes use of a function from `pagedir.c` which creates a file to write to, and then writes the URL, depth, and html of the page to it. 

To scan a page for more URLs, the `crawler` makes use of the `webpage` module, which can get each URL sequentially on a given page. If a URL it finds is internal, and hasn't been seen before (by checking it with the other keys in our hashtable), we can then add it to our bag of pages to check, and hashtable of URLs seen.

### Assumptions

There are not many assumptions outside of those outlined in the directions for the assignment.

### Compilation and Testing

`make` can be run to compile all needed files, `make test` can be run to test the crawler, `make valgrind` will test a short example with valgrind, and `make clean` can be used to delete all the files created by `make` or `make test`.