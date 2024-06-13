#include <stdlib.h>
#include <stdio.h>
#include "file_parser.h"

int getLineCount(const char* path);

int parseFile(const char* path) {

  
  int lineCount = getLineCount(path);

  if (lineCount < 0) {

    printf("ERROR");
    return -1;

  }

  struct Request requests[lineCount];

  return lineCount;

}

int getLineCount(const char* path) {

  // Open file
  FILE *file = fopen(path, "r");

  // Error Handling
  if (file == NULL) {
    // ERROR
    return -1;
  }

  // Count lines
  int counter = 0;

  for (;;) {

    char c = fgetc(file);
    if (feof(file)) break;
    if (c == 0) counter++;

  }

  // Close file & return
  fclose(file);

  return counter;

}