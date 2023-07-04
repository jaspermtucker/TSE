#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>

#include "pagedir.h"
#include "../libcs50/file.h"

/*
pagedir

Use: A helper module for crawler.c. Responsible for opening files to be written to and saving data to them.

Jasper Tucker, April 2023
*/

// creates a .crawler file within the directory where pages are saved and returns a bool
// with the status of doing so
bool pagedir_init(const char* pageDirectory)
{

    // allocate memory for path of directory to save pages to 
    char* filePath = malloc(sizeof(char) * (strlen(pageDirectory) + strlen("/.crawler") + 1));
    if (filePath == NULL){
        fprintf(stderr, "not enough memory to create path to file to write to\n");
        exit(2);
    }
    // add part of path that leads to directory 
    strcpy(filePath, pageDirectory);
    // add name of file on end
    strcat(filePath, "/.crawler");
    // open file for writing
    FILE* file = fopen(filePath, "w");
    if (file == NULL){
        // free if unable
        free(filePath);
        return false;
    }

    fclose(file);
    free(filePath);
    return true;
}

// creates a file for the given page and writes the URL, depth, and html to it
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID)
{
    // allocate enough memory for page path, being generous with size of integer
    char* pagePath = malloc(sizeof(char) * strlen(pageDirectory) + 15);
     if (pagePath == NULL){
        fprintf(stderr, "not enough memory to create path to write a webpage\n");
        exit(2);
    }

    // copy directory into path
    strcpy(pagePath, pageDirectory);

    // create string from id to put into path
    char* id = malloc(15);
    sprintf(id, "%d", docID);
    strcat(strcat(pagePath, "/"), id);

    free(id);

    // open file for writing
    FILE* file = fopen(pagePath, "w");
    if (file == NULL){
        // free if unable
        free(pagePath);
        fprintf(stderr, "was unable to open file for writing\n");
        exit(3);
    }

    // print URL, depth, and html to file
    char* URL = webpage_getURL(page);
    if (URL != NULL)
        fprintf(file, "%s\n", URL);

    int depth = webpage_getDepth(page);
    fprintf(file, "%d\n", depth);

    char* html = webpage_getHTML(page);
    if (html != NULL)
        fprintf(file, "%s\n", html);

    fclose(file);
    free(pagePath);

    //printf("SAVING: page id %d saved to file\n", docID);
}

// check that the given directory is one produced by crawler
bool pagedir_validate(const char* pageDirectory)
{
    char* filePath = malloc(sizeof(char) * (strlen(pageDirectory) + strlen("/.crawler") + 1));
    // add part of path that leads to directory 
    strcpy(filePath, pageDirectory);
    // add name of file on end
    strcat(filePath, "/.crawler");

    // if can access, file is there
    if (access(filePath, F_OK) == 0)
    {
        free(filePath);
        return true;
    }

    free(filePath);
    return false;

}

// creates a webpage from a file in the page directory and returns it
webpage_t* pagedir_load(const char* pageDirectory, int docID)
{
    // allocate enough memory for page path, being generous with size of integer
    char* pagePath = malloc(sizeof(char) * strlen(pageDirectory) + 15);
     if (pagePath == NULL){
        fprintf(stderr, "not enough memory to create path to write a webpage\n");
        exit(2);
    }

    // copy directory into path
    strcpy(pagePath, pageDirectory);

    // create string from id to put into path
    char* id = malloc(15);
    sprintf(id, "%d", docID);
    strcat(strcat(pagePath, "/"), id);
    free(id);

    // read file
    FILE* file = fopen(pagePath, "r");
    // if null, we have probably reached final page
    if (file == NULL){
        // free if unable
        free(pagePath);
        return NULL;
    }

    // get elements to create webpage
    // read first line for URL
    char* URL = file_readLine(file);

    // reading a file again discards last line
    char* docDepth = file_readLine(file);
    int depth;
    sscanf(docDepth, "%d", &depth);
    free(docDepth);

    // need html for webpage_getNextWord() laterm readFile goes to EOF,
    // reading file again discards last 2 lines
    char* html = file_readFile(file);

    fclose(file);
    free(pagePath);

    webpage_t* webpage = webpage_new(URL, depth, html);
    if (webpage == NULL){
        fprintf(stderr, "not enough memory to create webpage\n");
        exit(2);
    }

    return webpage;

}

