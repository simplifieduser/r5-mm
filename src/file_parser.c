#include <stdlib.h>
#include <stdio.h>
#include "file_parser.h"
#include "messages.h"

#define ERR_ALLOC (-1)
#define ERR_OPEN (-2)
#define ERR_EOF (-3)
#define ERR_NL (-4)
#define ERR_IA (-5)
#define ERR_TMA (-6)


int getRWArg(FILE *file);
int getAddressArg(FILE *file, uint32_t *res);
int getDataArg(FILE *file, uint32_t *res, int writeEnable);

void printError(int code, const char* arg, int line) {

    switch (code) {
        case ERR_ALLOC:
            printf(ERR_GENERAL_MEMORY_ALLOCATION_ERROR);
            break;
        case ERR_OPEN:
            printf(ERR_GENERAL_CANT_OPEN_FILE(arg));
            break;
        case ERR_EOF:
            printf(ERR_FILE_PREMATURE_END_OF_FILE(arg, line));
            break;
        case ERR_NL:
            printf(ERR_FILE_PREMATURE_NEW_LINE(arg, line));
            break;
        case ERR_IA:
            printf(ERR_FILE_INVALID_ARG(arg, line));
            break;
        case ERR_TMA:
            printf(ERR_FILE_TOO_MANY_ARGS(line));
            break;
        default:
            printf("CODE: %d\n", code);
            printf(ERR_GENERAL_UNKNOWN);
            break;
    }
}

int getLineCount(const char *path) {

    // Open file
    FILE *file = fopen(path, "r");

    if (file == NULL) {
        // IO ERROR
        printError(ERR_OPEN, path, 0);
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
        printError(ERR_ALLOC, "", 0);
        return -1;
    }

    uint32_t* data = malloc(sizeof(uint32_t));
    if (data == NULL) {
        // ERROR
        printError(ERR_ALLOC, "", 0);
        return -1;
    }

    // Open file
    FILE *file = fopen(path, "r");

    if (file == NULL) {
        // IO ERROR
        printError(ERR_OPEN, "", 0);
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
            printError(mode, "write_enable", i + 1);
            break;
        }

        // address arg
        int addressStatus = getAddressArg(file, address);
        if (addressStatus < 0) {
            // PARSING ERROR
            printError(addressStatus, "address", i + 1);
            break;
        }

        // data arg
        int dataStatus = getDataArg(file, data, mode);
        if (dataStatus < 0) {
            // PARSING ERROR
            printError(dataStatus, "write_data", i + 1);
            break;
        }



        // Instance new request struct & add to array
        Request newRequest;
        newRequest.we = mode;
        newRequest.addr = *address;
        newRequest.data = *data;

        requests[i] = newRequest;

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
        return ERR_NL;
    }

    // Get cell separator char

    int sepChar = fgetc(file);

    if (feof(file)) {
        // PARSING ERROR: PREMATURE END OF FILE
        return ERR_EOF;
    } else if (sepChar == '\n') {
        // PARSING ERROR: PREMATURE NEW LINE
        return ERR_NL;
    }

    if (sepChar != ',') {
        // PARSING ERROR: INVALID ARG
        return ERR_IA;
    }

    // Return mode, READ=0 WRITE=1

    if (modeChar == 'r' || modeChar == 'R') {
        return 0;
    } else if (modeChar == 'w' || modeChar == 'W') {
        return 1;
    } else {
        // PARSING ERROR: INVALID ARG
        return ERR_IA;
    }

}

int getAddressArg(FILE *file, uint32_t *res) {

    // Init address_string array

    char *address_string = malloc(sizeof(char) * 11);

    if (address_string == NULL) {
        // MEMORY ALLOC ERROR
        return ERR_ALLOC;
    }

    // Read in until ',' '\n' or 11 chars

    for (int i = 0; i < 11; i++) {

        int current = fgetc(file);

        if (feof(file)) {
            // PARSING ERROR: PREMATURE END OF FILE
            return ERR_EOF;
        }

        if (current == '\n') {
            // PARSING ERROR: PREMATURE NEW LINE
            return ERR_NL;
        }

        if (i == 10 && current != ',') {
            // PARSING ERROR: INVALID ARG
            return ERR_IA;
        }

        if (current == ',') {

            // if empty
            if (i == 0) {
                // PARSING ERROR: INVALID ARG
                return ERR_IA;
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
        return ERR_IA;
    }

    if (address_int > 0xFFFFFFFF) {
        // PARSING ERROR: INVALID ARG
        return ERR_IA;
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
            return ERR_EOF;
        }
        if (next != '\n') {
            // PARSING ERROR: TOO_MANY_ARGS
            return ERR_TMA;
        }
        return 0;
    }

    // Init data_string array

    char *data_string = malloc(sizeof(char) * 11);

    if (data_string == NULL) {
        // MEMORY ALLOC ERROR
        return ERR_ALLOC;
    }

    // Read in until ',' '\n' or 11 chars

    for (int i = 0; i < 11; i++) {

        int current = fgetc(file);

        if (feof(file)) {
            // PARSING ERROR: PREMATURE END OF FILE
            return ERR_EOF;
        }

        if (current == ',') {
            // PARSING ERROR: TOO MANY ARGS
            return ERR_TMA;
        }

        if (i == 10 && current != '\n') {
            // PARSING ERROR: INVALID ARG
            return ERR_IA;
        }

        if (current == '\n') {

            // if empty and not allowed
            if (i == 0) {
                // PARSING ERROR: INVALID ARG
                return ERR_IA;
            }

            // Append null byte
            data_string[i] ='\0';
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
        return ERR_IA;
    }

    if (data_int > 0xFFFFFFFF) {
        // PARSING ERROR: INVALID ARG
        return ERR_IA;
    }

    // Return address
    *res = data_int;
    return 0;

}