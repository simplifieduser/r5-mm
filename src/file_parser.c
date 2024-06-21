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

        // Read address

        uint32_t* address = malloc(sizeof(uint32_t));
        if (address == NULL) {
            // ERROR
            printf("GENERAL: ERROR - memory allocation error\n");
            return -1;
        }

        int status = getAddressArg(file, address);
        if (status < 0) {
            return -1;
        }

        printf("ADDRESS: %d\n", *address);

    }
    else if (mode == 1) {

        // Read address

        uint32_t* address = malloc(sizeof(uint32_t));
        if (address == NULL) {
            // ERROR
            printf("GENERAL: ERROR - memory allocation error\n");
            return -1;
        }

        int status = getAddressArg(file, address);
        if (status < 0) {
            return -1;
        }

        printf("ADDRESS: %d\n", *address);

    }
    else {
      fclose(file);
      return -1;
    }

  }

  return 0;

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

int getRWArg(FILE *file) {

  // Get mode argument char

  int modeChar = fgetc(file);

  if (feof(file)) {
    // PARSING ERROR: PREMATURE END OF FILE
    printf("ARG_RW: ERROR - premature end of file\n");
    return -1;
  }
  else if (modeChar == '\n') {
    // PARSING ERROR: PREMATURE NEW LINE
    printf("ARG_RW: ERROR - premature end of line\n");
    return -1;
  }

  // Get cell separator char

  int sepChar = fgetc(file);

  if (feof(file)) {
    // PARSING ERROR: PREMATURE END OF FILE
    printf("ARG_RW: ERROR - premature end of file\n");
    return -1;
  }
  else if (sepChar == '\n') {
    // PARSING ERROR: PREMATURE NEW LINE
    printf("ARG_RW: ERROR - premature end of line\n");
    return -1;
  }

  if (sepChar != ',') {
    // PARSING ERROR: ARGUMENT TOO LONG
    printf("ARG_RW: ERROR - argument too long\n");
    return -1;
  }

  // Return mode, READ=0 WRITE=1

  if (modeChar == 'r' || modeChar == 'R') {
    return 0;
  }
  else if (modeChar == 'w' || modeChar == 'W') {
    return 1;
  }
  else {
    // PARSING ERROR: NO VALID MODE
    printf("ARG_RW: ERROR - %c is not a valid mode\n", (char)modeChar);
    return -1;
  }

}
