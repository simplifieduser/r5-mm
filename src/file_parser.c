#include <stdlib.h>
#include <stdio.h>
#include "file_parser.h"


int getRWArg(FILE *file);
int getAddressArg(FILE *file, uint32_t *res);
int getDataArg(FILE *file, uint32_t *res, int writeEnable);

int getLineCount(const char *path) {

    // Open file
    FILE *file = fopen(path, "r");

    if (file == NULL) {
        // IO ERROR
        printf("IO: ERROR - can't open file '%s'\n", path);
        return -1;
    }

    // Count lines
    int counter = 1;

    while (1) {

        int c = fgetc(file);
        if (feof(file)) break;
        if (c == '\n') counter++;

    }

    // Close file & return
    fclose(file);
    return counter;

}

int parseFile(const char *path, int maxRequestCount, Request requests[]) {

    // Init value pointers
    uint32_t* address = malloc(sizeof(uint32_t));
    if (address == NULL) {
        // ERROR
        printf("GENERAL: ERROR - memory allocation error\n");
        return -1;
    }

    uint32_t* data = malloc(sizeof(uint32_t));
    if (data == NULL) {
        // ERROR
        printf("GENERAL: ERROR - memory allocation error\n");
        return -1;
    }

    // Open file
    FILE *file = fopen(path, "r");

    if (file == NULL) {
        // IO ERROR
        printf("IO: ERROR - can't open file '%s'\n", path);
        return -1;
    }

    // Read loop
    for (int i = 0; i < maxRequestCount; i++) {

        // r/w arg
        int mode = getRWArg(file);

        // Check if parsing complete
        if (mode == 2) {
            free(address);
            free(data);
            fclose(file);
            return i;
        }

        if (mode < 0) {
            // PARSING ERROR
            break;
        }

        // address arg
        int addressStatus = getAddressArg(file, address);
        if (addressStatus < 0) {
            // PARSING ERROR
            break;
        }

        // data arg
        int dataStatus = getDataArg(file, data, mode);
        if (dataStatus < 0) {
            // PARSING ERROR
            break;
        }

        // Instance new request struct & add to array
        Request newRequest;
        newRequest.we = mode;
        newRequest.addr = *address;
        newRequest.data = *data;

        requests[i] = newRequest;

        printf("MODE: %b\nADDRESS: %d\nDATA:%d\n", mode, *address, *data);

    }

    // Something failed
    free(address);
    free(data);
    fclose(file);
    return -1;

}

int getRWArg(FILE *file) {

    // Get mode argument char

    int modeChar = fgetc(file);

    // Check if file has ended, Return mode = 2
    if (feof(file)) {
        return 2;
    }

    if (modeChar == '\n') {
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
            printf("ARG_ADD: ERROR - address too long\n");
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

    if (address_int > 0xFFFFFFFF) {
        // PARSING ERROR: data too long
        printf("ARG_ADD: ERROR - empty argument\n");
        return -1;
    }

    // Return address
    *res = address_int;
    return 0;

}

int getDataArg(FILE *file, uint32_t *res, int writeEnable) {

    // Handle write mode disabled

    if (!writeEnable) {
        int next = fgetc(file);
        if (feof(file)) {
            // PARSING ERROR: PREMATURE END OF FILE
            printf("ARG_DATA: ERROR - premature end of file\n");
            return -1;
        }
        if (next != '\n') {
            // PARSING ERROR: WHEN WRITE DISABLED DATA NEEDS TO BE EMPTY
            printf("ARG_DATA: ERROR - data needs to be empty when reading\n");
            return -1;
        }
        return 0;
    }

    // Init data_string array

    char *data_string = malloc(sizeof(char) * 11);

    if (data_string == NULL) {
        // ERROR
        printf("GENERAL: ERROR - memory allocation error\n");
        return -1;
    }

    // Read in until ',' '\n' or 11 chars

    for (int i = 0; i < 11; i++) {

        int current = fgetc(file);

        if (feof(file)) {
            // PARSING ERROR: PREMATURE END OF FILE
            printf("ARG_DATA: ERROR - premature end of file\n");
            return -1;
        }

        if (current == ',') {
            // PARSING ERROR: ANOTHER ARG IN LINE
            printf("ARGS: ERROR - too many arguments in row\n");
            return -1;
        }

        if (i == 10 && current != '\n') {
            // PARSING ERROR: DATA TO LONG
            printf("ARG_DATA: ERROR - data too long\n");
            return -1;
        }

        if (current == '\n') {

            // if empty and not allowed
            if (i == 0) {
                // PARSING ERROR: EMPTY ADDRESS
                printf("ARG_DATA: ERROR - empty argument\n");
                return -1;
            }

            // Append null byte
            data_string[i] = 0;
            break;

        }

        // Append current char
        data_string[i] = (char) current;

    }

    // Convert string to int

    uint32_t data_int = 0;
    char *end;

    if (data_string[0] == '0' && data_string[1] == 'x') {
        data_int = strtol(data_string, &end, 16);
    } else {
        data_int = strtol(data_string, &end, 10);
    }

    if (*end != 0) {
        // PARSING ERROR: INVALID ADDRESS
        printf("ARG_DATA: ERROR - '%s' is not valid data\n", data_string);
        return -1;
    }

    if (data_int > 0xFFFFFFFF) {
        // PARSING ERROR: data too long
        printf("ARG_DATA: ERROR - data too long\n");
        return -1;
    }

    // Return address
    *res = data_int;
    return 0;

}