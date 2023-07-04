#include <stdbool.h>
#include "../libcs50/webpage.h"

/*
pagedir

Use: A helper module for crawler.c. Responsible for opening files to be written to and saving data to them.

Jasper Tucker, April 2023
*/

/**************** pagedir_init ****************/
/* 
Creates a .crawler file within the directory where pages are saved and returns a bool
with the status of doing so 

Inputs: a pageDirectory to write the file (string)
Outputs: a bool indicating the status of creating the file
*/
bool pagedir_init(const char* pageDirectory);

/**************** pagedir_save ****************/
/* 
Creates a file for the given page and writes the URL, depth, and html to it

Inputs: the page to save (webpage_t*), the pageDirectory to save it to (string), and the docID of the page (int)
Outputs: Creates a file with the page contents
*/
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

/**************** pagedir_validate ****************/
/* 
Checks that the given directory is one produced by crawler

Inputs: the page directory to check
Outputs: a bool status indicating if the directory is produced from the crawler or not
*/
bool pagedir_validate(const char* pageDirectory);

/**************** pagedir_load ****************/
/* 
Creates a webpage from a file in the page directory and returns it

Inputs: a page directory where pages are stored, and a docID of the page you want to find
Outputs: a webpage created from the page within the page directory
*/
webpage_t* pagedir_load(const char* pageDirectory, int docID);