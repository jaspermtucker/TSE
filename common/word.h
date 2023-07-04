#include <stdio.h>

/*
word

Use: A module providing a function to normalize a word

Jasper Tucker, April - May 2023
*/
/**************** normalize_word ****************/
/* 
Turns a word (char*) into all lowercase

Inputs: the word to turn to lowercase
Outputs: the same word, now in lowercase

Note: memory is not allocated in function, but given char* is altered
*/
char* normalize_word(char* word);