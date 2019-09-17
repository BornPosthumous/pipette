#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "line.h"

#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

// How to create/allocate a new struct with a factory function
ArrayWithLength* newLenArr( int size, char **arr){
  // Allocate space in the heap
  ArrayWithLength* p = malloc(sizeof(ArrayWithLength));
  // Set values
  p->size = size;
  p->arr = arr;
  //Return pointer to lenArr
  return p;
}

// This function returns an array of character pointers
ArrayWithLength* lsh_split_line(char *line){
  int bufsize = LSH_TOK_BUFSIZE,
      position = 0;
  // Creates 64 bytes of space to read arguments into
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;
  int numtokens = 0;
  if(!tokens){
    // fprintf allows you to push the string to stderr or file instead of stdout
    fprintf(stderr, "lsh: allocation error \n");
    exit(EXIT_FAILURE);
  }
// Splits str[] according to given delimiters.
// and returns next token. It needs to be called
// in a loop to get all tokens. It returns NULL
// when there are no more tokens.
// What strtok() actually does is return pointers to within the string you give it,
// and place \0 bytes at the end of each token. We store each pointer in an
// array (buffer) of character pointers.

  // Token should be a pointer to the end of the first occurence
  token = strtok(line, LSH_TOK_DELIM);// char * strtok(char str[], const char *delims);

  while(token != NULL){
    // Store the position (pointer address) of the token into the tokens array
    tokens[position] = token;
    position++;
    numtokens++;
    // If next position is greater than buffer size, reallocate
    if(position >= bufsize){
      // Make buffer beeg
      bufsize += LSH_TOK_BUFSIZE;
      // Realloc changes the size of the memory block pointed at <b>by pointer</b>
      // Increase tokens by the buffersize amount (64)
      tokens = realloc(tokens, bufsize * sizeof(char*));
      // If we cant reallocate that shiz throw an error
      if(!tokens){
        fprintf(stderr, "lsh: allocation error \n");
        exit(EXIT_FAILURE);
      }
    }
    /* fprintf(stdout,"token: %s \n", token); */
    // Subsequent calls to the tokenizer take NULL as a parameter
    token = strtok(NULL, LSH_TOK_DELIM);
  }
  // Terminate the string here
  tokens[position] = NULL;
  /* fprintf(stdout, "tokensize: %d  charsize %d \n" , (int)sizeof(tokens), (int)sizeof(char*)); */
  /* for(int i = 0; i < numtokens; i++ ){ */
  /*   fprintf(stdout, "arg: %s i: %d \n", tokens[i], i ); */
  /* } */
  return (newLenArr(numtokens, tokens));
}
