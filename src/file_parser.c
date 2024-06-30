#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "file_parser.h"
#include "messages.h"

// Used for determining success of supplement functions for arguments
enum RET_CODE {
    OK_READ, OK_WRITE, OK_EOF,                                                    // Codes solely used by getREArg
    OK, ERR_ALLOC, ERR_FOPEN, ERR_EOF, ERR_NEWLINE, ERR_INVARG, ERR_TOOMANY       // Codes used by all other args
} typedef RET_CODE;

const int MAX_ARG_LENGTH = 11;                    // 32-bit number can only consist of 11 chars
const unsigned int MAX_ARG_VALUE = 0xFFFFFFFF;    // Masked used to determining if passed number exceeds maximum

RET_CODE getRWArg(FILE *file);

RET_CODE getAddressArg(FILE *file, uint32_t *res, RET_CODE mode);

RET_CODE getDataArg(FILE *file, uint32_t *res);

void printError(RET_CODE code, const char *arg, int line) {

    switch (code) {
        case ERR_ALLOC:
            (void) fprintf(stderr, ERR_GENERAL_MEMORY_ALLOCATION_ERROR);
            break;
        case ERR_FOPEN:
            (void) fprintf(stderr, ERR_GENERAL_CANT_OPEN_FILE(arg));
            break;
        case ERR_EOF:
            (void) fprintf(stderr, ERR_FILE_PREMATURE_END_OF_FILE(arg, line));
            break;
        case ERR_NEWLINE:
            (void) fprintf(stderr, ERR_FILE_PREMATURE_NEW_LINE(arg, line));
            break;
        case ERR_INVARG:
            (void) fprintf(stderr, ERR_FILE_INVALID_ARG(arg, line));
            break;
        case ERR_TOOMANY:
            (void) fprintf(stderr, ERR_FILE_TOO_MANY_ARGS(line));
            break;
        default:
            (void) fprintf(stderr, ERR_GENERAL_UNKNOWN);
            break;
    }
}

int getLineCount(const char *path) {

    // Open file
    FILE *file = fopen(path, "re");

    if (file == NULL) {
        // IO ERROR
        printError(ERR_FOPEN, path, 0);
        return -1;
    }

    // Count lines
    int counter = 1;

    while (1) {

        int current = fgetc(file);
        if (feof(file)) { break; }
        if (current == '\n') { counter++; }

    }

    // Close file & return
    if (fclose(file) != 0) {
        // IO ERROR
        return -1;
    }
    return counter;

}

int parseFile(const char *path, int maxRequestCount, Request requests[]) {

    // Init value pointers
    uint32_t *address = malloc(sizeof(uint32_t));
    if (address == NULL) {
        // ERROR
        printError(ERR_ALLOC, "", 0);
        return -1;
    }

    uint32_t *data = malloc(sizeof(uint32_t));
    if (data == NULL) {
        // MEMORY ERR_ALLOC ERROR
        free(address);
        printError(ERR_ALLOC, "", 0);
        return -1;
    }

    // Open file
    FILE *file = fopen(path, "re");

    if (file == NULL) {
        // IO ERROR
        free(address);
        free(data);
        printError(ERR_FOPEN, "", 0);
        return -1;
    }

    // Read loop
    for (int i = 0; i < maxRequestCount; i++) {

        // r/w arg
        int mode = getRWArg(file);

        // Check if parsing complete
        if (mode == OK_EOF) {
            free(address);
            free(data);

            if (fclose(file) != 0) {
                // IO ERROR
                return -1;
            }

            return i;
        }

        if (mode != OK_READ && mode != OK_WRITE) {
            // PARSING ERROR
            printError(mode, "write_enable", i + 1);
            break;
        }

        // address arg
        int addressStatus = getAddressArg(file, address, mode);
        if (addressStatus != OK) {
            // PARSING ERROR
            printError(addressStatus, "address", i + 1);
            break;
        }

        // data arg - if write enabled
        if (mode == OK_WRITE) {
            int dataStatus = getDataArg(file, data);
            if (dataStatus != OK) {
                // PARSING ERROR
                printError(dataStatus, "write_data", i + 1);
                break;
            }
        }

        // Instance new request struct & add to array
        Request newRequest = {.we=mode, .data=*address, .addr=0};
        if (mode == OK_WRITE) {
            newRequest.data = *data;
        }

        requests[i] = newRequest;

    }

    // Something failed
    free(address);
    free(data);
    if (fclose(file) != 0) {
        // IO ERROR
        return -1;
    }
    return -1;

}

RET_CODE getRWArg(FILE *file) {

    // Get mode argument char

    int modeChar = fgetc(file);

    // Check if file has ended
    if (feof(file)) {
        return OK_EOF;
    }

    if (modeChar == '\n') {
        // PARSING ERROR: PREMATURE NEW LINE
        return ERR_NEWLINE;
    }

    // Get cell separator char

    int sepChar = fgetc(file);

    if (feof(file)) {
        // PARSING ERROR: PREMATURE END OF FILE
        return ERR_EOF;
    }

    if (sepChar == '\n') {
        // PARSING ERROR: PREMATURE NEW LINE
        return ERR_NEWLINE;
    }

    if (sepChar != ',') {
        // PARSING ERROR: INVALID ARG
        return ERR_INVARG;
    }

    // Return mode

    if (modeChar == 'r' || modeChar == 'R') {
        return OK_READ;
    }

    if (modeChar == 'w' || modeChar == 'W') {
        return OK_WRITE;
    }

    // PARSING ERROR: INVALID ARG
    return ERR_INVARG;

}

RET_CODE getAddressArg(FILE *file, uint32_t *res, RET_CODE mode) {

    // Init address_string array

    char *address_string = malloc(sizeof(char) * MAX_ARG_LENGTH);

    if (address_string == NULL) {
        // MEMORY ALLOC ERROR
        return ERR_ALLOC;
    }

    // Read in until ',' '\n' or 11 chars

    for (int i = 0; i < MAX_ARG_LENGTH; i++) {

        int current = fgetc(file);

        if (feof(file)) {
            // PARSING ERROR: PREMATURE END OF FILE
            free(address_string);
            return ERR_EOF;
        }

        // On new line in read mode -> valid
        if (current == '\n' && mode == OK_READ) {

            // if empty
            if (i == 0) {
                // PARSING ERROR: INVALID ARG
                free(address_string);
                return ERR_INVARG;
            }

            // Append null byte
            address_string[i] = 0;
            break;

        }

        // On new line in write mode -> invalid
        if (current == '\n' && mode == OK_WRITE) {
            // PARSING ERROR: PREMATURE NEW LINE
            free(address_string);
            return ERR_NEWLINE;
        }

        if (i == MAX_ARG_LENGTH - 1 && current != ',') {
            // PARSING ERROR: INVALID ARG
            free(address_string);
            return ERR_INVARG;
        }

        if (current == ',') {

            // if empty
            if (i == 0) {
                // PARSING ERROR: INVALID ARG
                free(address_string);
                return ERR_INVARG;
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
        // PARSING ERROR: INVALID ARG
        free(address_string);
        return ERR_INVARG;
    }

    if (address_int > MAX_ARG_VALUE) { // TODO: IS NECESSARY??
        // PARSING ERROR: INVALID ARG
        free(address_string);
        return ERR_INVARG;
    }

    // Return address
    *res = address_int;
    free(address_string);
    return OK;

}

RET_CODE getDataArg(FILE *file, uint32_t *res) {

    // Init data_string array

    char *data_string = malloc(sizeof(char) * MAX_ARG_LENGTH);

    if (data_string == NULL) {
        // MEMORY ERR_ALLOC ERROR
        free(data_string);
        return ERR_ALLOC;
    }

    // Read in until ',' '\n' or 11 chars

    for (int i = 0; i < MAX_ARG_LENGTH; i++) {

        int current = fgetc(file);

        if (feof(file)) {
            // PARSING ERROR: PREMATURE END OF FILE
            free(data_string);
            return ERR_EOF;
        }

        if (current == ',') {
            // PARSING ERROR: TOO MANY ARGS
            free(data_string);
            return ERR_TOOMANY;
        }

        if (i == MAX_ARG_LENGTH - 1 && current != '\n') {
            // PARSING ERROR: INVALID ARG
            free(data_string);
            return ERR_INVARG;
        }

        if (current == '\n') {

            // if empty
            if (i == 0) {
                // PARSING ERROR: INVALID ARG
                free(data_string);
                return ERR_INVARG;
            }

            // Append null byte
            data_string[i] = '\0';
            break;

        }

        // Append current char
        data_string[i] = (char) current;

    }

    uint32_t data_int = 0;
    char *end;

    if (data_string[0] == '0' && data_string[1] == 'x') {
        data_int = strtol(data_string, &end, 16);
    } else {
        data_int = strtol(data_string, &end, 10);
    }

    if (*end != '\0') {
        // PARSING ERROR: INVALID ARG
        free(data_string);
        return ERR_INVARG;
    }

    if (data_int > MAX_ARG_VALUE) { // TODO: IS NECESSARY??
        // PARSING ERROR: INVALID ARG
        free(data_string);
        return ERR_INVARG;
    }

    // Return address
    *res = data_int;
    free(data_string);
    return OK;

}
