#include <stdlib.h>
#include <stdio.h>
#include "file_parser.h"

int getLineCount(const char *path);

int getRWArg(FILE *file);

int getAddressArg(FILE *file, uint32_t *res);

int parseFile(const char *path) {

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

        // r/w arg
        int mode = getRWArg(file);
        if (mode < 0) {

            fclose(file);
            return -1;
        }

        // address arg

        uint32_t* address = malloc(sizeof(uint32_t));
        if (address == NULL) {
            // ERROR
            printf("GENERAL: ERROR - memory allocation error\n");
            free(address);
            return -1;
        }

        int status = getAddressArg(file, address);
        if (status < 0) {
            free(address);
            return -1;
        }

        printf("ADDRESS: %d\n", *address);

    }

    return 0;

}

int getLineCount(const char *path) {

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
    } else if (modeChar == '\n') {
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
    } else if (sepChar == '\n') {
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
    } else if (modeChar == 'w' || modeChar == 'W') {
        return 1;
    } else {
        // PARSING ERROR: NO VALID MODE
        printf("ARG_RW: ERROR - %c is not a valid mode\n", (char) modeChar);
        return -1;
    }

}

int getAddressArg(FILE *file, uint32_t *res) {

    // Init address_string array

    char *address_string = malloc(sizeof(char) * 11);

    if (address_string == NULL) {
        // ERROR
        printf("GENERAL: ERROR - memory allocation error\n");
        return -1;
    }

    // Read in until ',' '\n' or 11 chars

    for (int i = 0; i < 11; i++) {

        int current = fgetc(file);

        if (feof(file)) {
            // PARSING ERROR: PREMATURE END OF FILE
            printf("ARG_ADD: ERROR - premature end of file\n");
            return -1;
        }

        if (current == '\n') {
            // PARSING ERROR: PREMATURE END OF LINE
            printf("ARG_ADD: ERROR - premature end of line\n");
            return -1;
        }

        if (i == 10 && current != ',') {
            // PARSING ERROR: ADDRESS TO LONG
            printf("ARG_ADD: ERROR - address_string too long\n");
            return -1;
        }

        if (current == ',') {

            // if empty
            if (i == 0) {
                // PARSING ERROR: EMPTY ADDRESS
                printf("ARG_ADD: ERROR - empty argument\n");
                return -1;
            }

            // Append null byte
            address_string[i] = 0;
            break;

        }

        // Append current char
        address_string[i] = (char) current;

    }

    // Convert string to int

    uint32_t address_int = 0;
    char *end;

    if (address_string[0] == '0' && address_string[1] == 'x') {
        address_int = strtol(address_string, &end, 16);
    } else {
        address_int = strtol(address_string, &end, 10);
    }

    if (*end != 0) {
        // PARSING ERROR: INVALID ADDRESS
        printf("ARG_ADD: ERROR - '%s' is not an valid address\n", address_string);
        return -1;
    }

    // Return address
    *res = address_int;
    return 0;

}