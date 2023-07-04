#include <stdio.h>
#include <ctype.h>

/*
word

Use: A module providing a function to normalize a word

Jasper Tucker, April - May 2023
*/

// turns a word (char*) into all lowercase
char* normalize_word(char* word)
{
    int i = 0;

     while (word[i]) { 
        word[i] = tolower(word[i]);  
        i++;
    }

    return word;
}