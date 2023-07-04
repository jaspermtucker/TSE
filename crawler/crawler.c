#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common/pagedir.h"
#include "../libcs50/bag.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/webpage.h"

/*
crawler

Input: A URL (string), a directory filepath (string), and a depth (int)
Output: Creates files within a specifed directory containing data from pages found while crawling URLs from
the first given URL (log messages to stdout can also be uncommented)

Use: The crawler begins at the URL given and writes to a file in the directory given. The file contains the URL,
html, and what 'depth' the page was at. Depth refers to the distance away from the first URL. The crawler then does the 
same process for each link it finds on that URL's webpage, and so on and so forth until depth has been reached.

There are available print statements that can be used for logging progress, but they are disabled so they don't bloat testing.out
Jasper Tucker, April 2023

EXIT CODES:
exit code 0: program ran as normal
exit code 1: argument error
exit code 2: could not allocate memory
exit code 3: issue initalizing module
*/


static void parseArgs(const int argc, char* argv[],
                      char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);

static void itemdelete(void* item);

int main(int argc, char* argv[])
{
    char* seedURL = NULL;
    char* pageDirectory = NULL;
    int maxDepth = 0;
    // pass in addresses to keep track of variables
    parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);
    crawl(seedURL, pageDirectory, maxDepth);
    return 0;
}

// check that each argument is correct and write them to their corresponding variables
static void parseArgs(const int argc, char* argv[],
                      char** seedURL, char** pageDirectory, int* maxDepth)
{
    // see if number of arguments was correct
    if (argc == 1) {
        fprintf(stderr, "No arguments were given - pass a URL to begin with, a directory to save pages to, and a max depth to traverse\n");
        exit(1);
    }
    else if (argc > 4){
         fprintf(stderr, "Too many arguments were given - pass a URL to begin with, a directory to save pages to, and a max depth to traverse\n");
         exit(1);
    }
    else {

        //check that argv[1] is good (seedURL)
        char* normalizedURL = normalizeURL(argv[1]);
        if (normalizedURL == NULL){
            fprintf(stderr, "URL passed (first arg) could not be normalized\n");
            exit(1);
        }
        if (!isInternalURL(normalizedURL)) {
            free(normalizedURL);
            fprintf(stderr, "URL passed (first arg) is not internal\n");
            exit(1);
        }
        // good to go, assign seedURL to argv[1]
        *seedURL = normalizedURL;

        // check that argv[2] is good (pageDirectory)
        if (!pagedir_init(argv[2])){
            fprintf(stderr, "page directory passed (second arg) could not be initialized\n");
            exit(1);
        }
        // good to go, assign pageDirectory to argv[2]
        *pageDirectory = argv[2];
        
        int depth;
        // check that argv[3] is good (maxDepth)
        if (sscanf(argv[3], "%d", &depth) != 1) {
            fprintf(stderr, "max depth passed (third arg) was not an integer\n");
            exit(1);
        }

        *maxDepth = depth;

        if (*maxDepth < 0 || *maxDepth > 10) {
            fprintf(stderr, "max depth passed (third arg) was not within range of 0 to 10 (inclusive)\n");
            exit(1);
        }

        // good to go, maxDepth is assigned to depth earlier
        printf("arguments checked\n");
    }
}

// starting with the first URL, go through all webpages calling functions to write their data to files,
// and checking each page if there are more pages to search
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth)
{ 
    // create hashtable for seen URLs
    hashtable_t* hashtableSeenURLs = hashtable_new(300);

    if (hashtableSeenURLs == NULL) {
        fprintf(stderr, "hashtable_new failed for hashtableSeenURLs\n");
        exit(3);
    }
    // add first seed URL to hashtable
    // using "" for item, only key (URL) really matters for searching purposes
    hashtable_insert(hashtableSeenURLs, seedURL, "");

    // create bag for webpages to explore
    bag_t* pagesToSearch = bag_new();
    if (pagesToSearch == NULL) {
        fprintf(stderr, "bag_new failed for pagesToSearch\n");
        exit(3);
    }

    // create webpage for seed URL
    webpage_t* firstPage = webpage_new(seedURL, 0, NULL);
    if (firstPage == NULL) {
        fprintf(stderr, "webpage_new failed for firstPage\n");
        exit(3);
    }

    // add first webpage to bag
    bag_insert(pagesToSearch, firstPage);

    int id = 1;
    // if there is something in the bag...
    webpage_t* webpage = NULL;
    while ((webpage = bag_extract(pagesToSearch)) != NULL) {

        // if we fetch the URL...
         if(webpage_fetch(webpage)) {
            // html is allocated by webpage.c
            // save webpage to pageDirectory
            pagedir_save(webpage, pageDirectory, id);

            // if not at max depth, look for URLs
            if (webpage_getDepth(webpage) < maxDepth) {
                pageScan(webpage, pagesToSearch, hashtableSeenURLs);
            }

            // id ready for next webpage
            id++;
        }

        // free webpage
        webpage_delete(webpage);
    }

    // free other data structures when done
    bag_delete(pagesToSearch, itemdelete);
    // do not delete items in hashtable, that is handled by webpage_delete
    hashtable_delete(hashtableSeenURLs, NULL);

    printf("\n-- end --\n\n");
}

// check for all URLs on the given page and add them to pages to check
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen)
{
    // while there is another URL...
    char* nextURL = NULL;
    int pos = 0;

    while ((nextURL = webpage_getNextURL(page, &pos)) != NULL) {
        
        //printf("FOUND: found URL\n");

        char* normalizedURL = normalizeURL(nextURL);
        if (normalizedURL == NULL) {
            //fprintf(stderr, "URL found on webpage could not be normalized, skipped\n");
            free(normalizedURL);
        }

        // if URL is internal...
        if (isInternalURL(normalizedURL)) {

            // hashtable_insert returns bool, check if worked/if it is already in
            if (hashtable_insert(pagesSeen, normalizedURL, "")) {

                // create webpage from URL at depth above page the URL is on, then add to bag
                webpage_t* webpage = webpage_new(normalizedURL, webpage_getDepth(page) + 1, NULL);
                bag_insert(pagesToCrawl, webpage);

                //printf("ADDING: URL not checked, adding to to-do list\n");
            } else {

                // free URL is it won't be freed by webpage later
                free(normalizedURL);

                //printf("SKIPPING: URL already checked, skipping\n");
            }
        } else {

            // free URL if it wasn't internal
            free(normalizedURL);

            //printf("SKIPPING: URL not internal, skipping\n");
        }

        // normalized URL is freed in webpage_delete later
        free(nextURL);
    }

}

// helper function to pass to bag_delete
static void itemdelete(void* item)
{
  if (item != NULL) {
    free(item);   
  }
}
