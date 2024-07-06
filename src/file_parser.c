#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "file_parser.h"
#include "messages.h"

// Wird zur Bestimmung des Erfolgs der Zusatzfunktionen für Argumente verwendet
enum RET_CODE {
    OK_READ, OK_WRITE, OK_EOF,                                                    // Codes, die ausschließlich von getREArg verwendet werden
    OK, ERR_ALLOC, ERR_FOPEN, ERR_EOF, ERR_NEWLINE, ERR_INVARG, ERR_TOOMANY       // Codes, die von allen anderen Argumenten verwendet werden
} typedef RET_CODE;

const int MAX_ARG_LENGTH = 11;                    // Eine 32-Bit-Zahl kann nur aus 11 Zeichen bestehen
const unsigned int MAX_ARG_VALUE = 0xFFFFFFFF;    // Maske zur Bestimmung, ob die übergebene Zahl den Höchstwert überschreitet

const int HEX_BASE = 16;                 // Basis für die Umwandlung von hexadezimalen Zahlen
const int DEC_BASE = 10;                 // Basis für die Umwandlung von dezimalen Zahlen

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

int parseFile(const char *path, Request **requests) {

    // Initialisiere Pointer
    uint32_t *address = malloc(sizeof(uint32_t));
    if (address == NULL) {
        // SPEICHER-FEHLER
        printError(ERR_ALLOC, "", 0);
        return -1;
    }

    uint32_t *data = malloc(sizeof(uint32_t));
    if (data == NULL) {
        // SPEICHER-FEHLER
        free(address);
        printError(ERR_ALLOC, "", 0);
        return -1;
    }

    // Datei öffnen
    FILE *file = fopen(path, "re");

    if (file == NULL) {
        // IO-FEHLER
        free(address);
        free(data);
        printError(ERR_FOPEN, "", 0);
        return -1;
    }

    // Lese-Schleife
    for (int i = 0;; i++) {

        // Lese-/Schreib-Argument
        int mode = getRWArg(file);

        // Prüfen, ob das Parsen abgeschlossen ist
        if (mode == OK_EOF) {
            free(address);
            free(data);

            if (fclose(file) != 0) {
                // IO-FEHLER
                return -1;
            }

            return i;
        }

        if (mode != OK_READ && mode != OK_WRITE) {
            // PARSE-FEHLER
            printError(mode, "write_enable", i + 1);
            break;
        }

        // Adress-Argument
        int addressStatus = getAddressArg(file, address, mode);
        if (addressStatus != OK) {
            // PARSE-FEHLER
            printError(addressStatus, "address", i + 1);
            break;
        }

        // Daten-Argument - wenn Schreibzugriff aktiviert
        if (mode == OK_WRITE) {
            int dataStatus = getDataArg(file, data);
            if (dataStatus != OK) {
                // PARSE-FEHLER
                printError(dataStatus, "write_data", i + 1);
                break;
            }
        }

        // Speicher für neue Request zuweisen und zum Array hinzufügen

        size_t newSize = 0;
        if (__builtin_mul_overflow((i + 1), sizeof(Request), &newSize)) {
            // SPEICHER-FEHLER ERR_ALLOC
            free(address);
            free(data);
            printError(ERR_ALLOC, "", 0);
            return -1;
        }

        *requests = realloc(*requests, newSize);

        if (*requests == NULL) {
            // SPEICHER-FEHLER
            free(address);
            free(data);
            printError(ERR_ALLOC, "", 0);
            return -1;
        }

        Request newRequest = {.we=mode, .addr=*address, .data=0};
        if (mode == OK_WRITE) {
            newRequest.data = *data;
        }

        (*requests)[i] = newRequest;

    }

    // Etwas ist fehlgeschlagen
    free(address);
    free(data);
    if (fclose(file) != 0) {
        // IO-FEHLER
        return -1;
    }
    return -1;

}

RET_CODE getRWArg(FILE *file) {

    // Lese mode Charakter

    int modeChar = fgetc(file);

    // Prüfen, ob die Datei zu Ende ist
    if (feof(file)) {
        return OK_EOF;
    }

    if (modeChar == '\n') {
        // PARSE-FEHLER: VORZEITIGE ZEILENENDE
        return ERR_NEWLINE;
    }

    // Lese Trenncharakter

    int sepChar = fgetc(file);

    if (feof(file)) {
        // PARSE-FEHLER: VORZEITIGES DATEIENDE
        return ERR_EOF;
    }

    if (sepChar == '\n') {
        // PARSE-FEHLER: VORZEITIGES ZEILENENDE
        return ERR_NEWLINE;
    }

    if (sepChar != ',') {
        // PARSE-FEHLER: UNGÜLTIGES ARGUMENT
        return ERR_INVARG;
    }

    // mode zurückgeben

    if (modeChar == 'r' || modeChar == 'R') {
        return OK_READ;
    }

    if (modeChar == 'w' || modeChar == 'W') {
        return OK_WRITE;
    }

    // PARSE-FEHLER: UNGÜLTIGES ARGUMENT
    return ERR_INVARG;

}

RET_CODE getAddressArg(FILE *file, uint32_t *res, RET_CODE mode) {

    // Initialisiere array fürs lesen

    char *address_string = malloc(sizeof(char) * MAX_ARG_LENGTH);

    if (address_string == NULL) {
        // SPEICHER-FEHLER
        return ERR_ALLOC;
    }

    // Einlesen bis ',' '\n' oder 11 Zeichen

    for (int i = 0; i < MAX_ARG_LENGTH; i++) {

        int current = fgetc(file);

        if (feof(file)) {
            // PARSE-FEHLER: VORZEITIGES DATEIENDE
            free(address_string);
            return ERR_EOF;
        }

        // Wenn neue Zeile im Lesemodus → gültig
        if (current == '\n' && mode == OK_READ) {

            // wenn leeres Argument
            if (i == 0) {
                // PARSE-FEHLER: UNGÜLTIGES ARGUMENT
                free(address_string);
                return ERR_INVARG;
            }

            // Null-Byte anhängen
            address_string[i] = 0;
            break;

        }

        // Wenn neue Zeile im Schreibmodus → ungültig
        if (current == '\n' && mode == OK_WRITE) {
            // PARSE-FEHLER: VORZEITIGER NEUE ZEILE
            free(address_string);
            return ERR_NEWLINE;
        }

        if (i == MAX_ARG_LENGTH - 1 && current != ',') {
            // PARSE-FEHLER: UNGÜLTIGES ARGUMENT
            free(address_string);
            return ERR_INVARG;
        }

        if (current == ',') {

            // wenn leeres Argument
            if (i == 0) {
                // PARSE-FEHLER: UNGÜLTIGES ARGUMENT
                free(address_string);
                return ERR_INVARG;
            }

            // Null-Byte anhängen
            address_string[i] = 0;
            break;

        }

        // Aktuelles Zeichen anhängen
        address_string[i] = (char) current;

    }

    // string in int konvertieren

    uint32_t address_int = 0;
    char *end = NULL;

    if (address_string[0] == '0' && address_string[1] == 'x') {
        address_int = strtol(address_string, &end, HEX_BASE);
    } else {
        address_int = strtol(address_string, &end, DEC_BASE);
    }

    if (*end != 0) {
        // PARSE-FEHLER: UNGÜLTIGES ARGUMENT
        free(address_string);
        return ERR_INVARG;
    }

    if (address_int > MAX_ARG_VALUE) {
        // PARSE-FEHLER: UNGÜLTIGES ARGUMENT
        free(address_string);
        return ERR_INVARG;
    }

    // Adresse zurückgeben
    *res = address_int;
    free(address_string);
    return OK;

}

RET_CODE getDataArg(FILE *file, uint32_t *res) {

    // Initialisiere array fürs lesen

    char *data_string = malloc(sizeof(char) * MAX_ARG_LENGTH);

    if (data_string == NULL) {
        // SPEICHER-FEHLER
        free(data_string);
        return ERR_ALLOC;
    }

    // Einlesen bis ',' '\n' oder 11 Zeichen

    for (int i = 0; i < MAX_ARG_LENGTH; i++) {

        int current = fgetc(file);

        if (feof(file)) {
            // PARSE-FEHLER: VORZEITIGES DATEIENDE
            free(data_string);
            return ERR_EOF;
        }

        if (current == ',') {
            // PARSE-FEHLER: ZU VIELE ARGUMENTE
            free(data_string);
            return ERR_TOOMANY;
        }

        if (i == MAX_ARG_LENGTH - 1 && current != '\n') {
            // PARSE-FEHLER: UNGÜLTIGES ARGUMENT
            free(data_string);
            return ERR_INVARG;
        }

        if (current == '\n') {

            // wenn leers Argument
            if (i == 0) {
                // PARSE-FEHLER: UNGÜLTIGES ARGUMENT
                free(data_string);
                return ERR_INVARG;
            }

            // Null-Byte anhängen
            data_string[i] = '\0';
            break;

        }

        // Aktuelles Zeichen anhängen
        data_string[i] = (char) current;

    }

    // string in int konvertieren

    uint32_t data_int = 0;
    char *end = NULL;

    if (data_string[0] == '0' && data_string[1] == 'x') {
        data_int = strtol(data_string, &end, HEX_BASE);
    } else {
        data_int = strtol(data_string, &end, DEC_BASE);
    }

    if (*end != '\0') {
        // PARSE-FEHLER: UNGÜLTIGES ARGUMENT
        free(data_string);
        return ERR_INVARG;
    }

    if (data_int > MAX_ARG_VALUE) {
        // PARSE-FEHLER: UNGÜLTIGES ARGUMENT
        free(data_string);
        return ERR_INVARG;
    }

    // Adresse zurückgeben
    *res = data_int;
    free(data_string);
    return OK;

}