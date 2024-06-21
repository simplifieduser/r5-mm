#include <stdlib.h>
#include <stdio.h>
#include "file_parser.h"

int getLineCount(const char* path);
int getRWArg(FILE *file);
int getAddressArg(FILE *file, uint32_t* res);

int parseFile(const char* path) {

  // Get line count
  int lineCount = getLineCount(path);
  if (lineCount < 0) {
    return -1;
  }

  // Init return values
  int reqCount = 0;
  struct Request requests[lineCount];

  // Open file 
  FILE *file = fopen(path, "r");

  if (file == NULL) {
    // IO ERROR
    printf("IO: ERROR - can't open file '%s'\n", path);
    return -1;
  }

  // Read loop
  while (1) {

    // First r/w arg
    int mode = getRWArg(file);
    if (mode == 0) {

    }
    else if (mode == 1) {

    }
    else {
      printf("ARG ERROR (%d)\n", mode);
      fclose(file);
      return -1;
    }

  }

  return 0;

}

int getRWArg(FILE *file) {

  int modeChar = fgetc(file);
  if (feof(file)) return -1;

  int sepChar = fgetc(file);
  if (feof(file) || sepChar != ',') return -2;

  if (modeChar == 'r' || modeChar == 'R') {
    return 0;
  }
  else if (modeChar == 'w' || modeChar == 'W') {
    return 1;
  }
  else {
    return -3;
  }

}

int getLineCount(const char* path) {

  // Open file
  FILE *file = fopen(path, "r");

  if (file == NULL) {
    // IO ERROR
    printf("IO: ERROR - can't open file '%s'\n", path);
    return -1;
  }

  // Count lines
  int counter = 0;
  int last = '\n';

  while (1) {

    int c = fgetc(file);
    if (feof(file)) break;
    if (c == '\n') counter++;
    last = c;

  }

  // Fail if last char is not new line
  if (last != '\n') {
    // PARSING ERROR: NO NEW LINE AT END OF FILE
    printf("IO: ERROR - no new line at end of file\n");
    fclose(file);
    return -1;
  }

  // Close file & return
  fclose(file);
  return counter;

}