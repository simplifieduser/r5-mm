#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include "shared.h"
#include "file_parser.h"

// Wird zur Bestimmung des Erfolgs der Zusatzfunktionen für Argumente verwendet
enum RetCode {
    OK_READ, OK_WRITE, OK_EOF, ERROR
} typedef RetCode;

const int MAX_ARG_LENGTH = 11;                    // Eine 32-Bit-Zahl kann nur aus 11 Zeichen bestehen
const unsigned int MAX_ARG_VALUE = 0xFFFFFFFF;    // Maske zur Bestimmung, ob die übergebene Zahl den Höchstwert überschreitet

const int HEX_BASE = 16;                 // Basis für die Umwandlung von hexadezimalen Zahlen
const int DEC_BASE = 10;                 // Basis für die Umwandlung von dezimalen Zahlen

RetCode getRWArg(FILE *file, size_t line);

int getAddressArg(FILE *file, uint32_t *res, size_t line);

int getDataArg(FILE *file, uint32_t *res, RetCode mode, size_t line);

int CORRECTED_CARR_RET = 0;  // Wird verwendet, um Korrektur von '\r' zu '\n' zu speichern

// Edge case: Falls für Zeilenumbrüche im CSV-Standard '\r\n' oder '\r' verwendet wird
int getNextChar(FILE *file) {

    // Wenn letzter char '\r', dann überspringe '\n'
    if (CORRECTED_CARR_RET) {
        int next = fgetc(file);
        if (next == '\n') {
            next = fgetc(file);
        }
        CORRECTED_CARR_RET = 0;
        return next;
    }

    // Korrigiere '\r' zu '\n'
    int next = fgetc(file);
    if (next == '\r') {
        next = '\n';
        CORRECTED_CARR_RET = 1;
    }

    return next;

}

int parseFile(const char *path, size_t *requestCount, Request **requests) {

    // Initialisiere Pointer
    uint32_t *address = malloc(sizeof(uint32_t));
    if (address == NULL) {
        // Fehler beim allokieren von Speicher
        (void) fprintf(stderr, ERR_GENERAL_MEMORY_ALLOCATION_ERROR);
        return -1;
    }

    uint32_t *data = malloc(sizeof(uint32_t));
    if (data == NULL) {
        // Fehler beim allokieren von Speicher
        free(address);
        (void) fprintf(stderr, ERR_GENERAL_MEMORY_ALLOCATION_ERROR);
        return -1;
    }

    // Datei öffnen
    FILE *file = fopen(path, "re");

    if (file == NULL) {
        // Fehler beim Öffnen der Datei
        free(address);
        free(data);
        fprintf(stderr, ERR_GENERAL_CANT_OPEN_FILE(path));
        return -1;
    }

    // Lese-Schleife
    for (size_t i = 0;; i++) {

        // Lese-/Schreib-Argument
        int mode = getRWArg(file, i + 1);

        // Prüfen, ob das Parsen abgeschlossen ist
        if (mode == OK_EOF) {
            free(address);
            free(data);

            if (fclose(file) != 0) {
                // Fehler beim Öffnen der Datei
                (void) fprintf(stderr, ERR_GENERAL_UNKNOWN);
                return -1;
            }

            // Setze Anzahl der Requests & gebe Erfolg zurück
            *requestCount = i;

            return 0;
        }

        if (mode == ERROR) {
            // Fehler beim Parsen
            break;
        }

        // Address-Argument
        int addressStatus = getAddressArg(file, address, i + 1);
        if (addressStatus) {
            // Fehler beim Parsen
            break;
        }

        // Daten-Argument
        int dataStatus = getDataArg(file, data, mode, i + 1);
        if (dataStatus) {
            // Fehler beim Parsen
            break;
        }

        // Speicher für neue Request zuweisen und zum Array hinzufügen

        size_t newSize = 0;
        if (__builtin_mul_overflow((i + 1), sizeof(Request), &newSize)) {
            // Fehler beim allokieren von Speicher
            (void) fprintf(stderr, ERR_GENERAL_MEMORY_ALLOCATION_ERROR);
            break;
        }

        *requests = realloc(*requests, newSize);

        if (*requests == NULL) {
            // Fehler beim allokieren von Speicher
            (void) fprintf(stderr, ERR_GENERAL_MEMORY_ALLOCATION_ERROR);
            break;
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
    (void) fclose(file);

    return -1;

}

RetCode getRWArg(FILE *file, size_t line) {

    // Lese mode Charakter

    int modeChar = getNextChar(file);

    // Prüfen, ob die Datei zu Ende ist
    if (feof(file)) {
        return OK_EOF;
    }

    if (modeChar == '\n') {
        // Fehler beim Parsen: Vorzeitiges Ende der Zeile
        (void) fprintf(stderr, ERR_FILE_PREMATURE_EOF_ARG("write_enable", line));
        return ERROR;
    }

    if (modeChar != 'r' && modeChar != 'R' && modeChar != 'w' && modeChar != 'W') {
        // Fehler beim Parsen: Ungültiges Argument
        (void) fprintf(stderr, ERR_FILE_INVALID_ARG_RW(line));
        return ERROR;
    }

    // Lese Trenncharakter

    int sepChar = getNextChar(file);

    if (feof(file)) {
        // Fehler beim Parsen: Vorzeitiges Ende der Datei
        (void) fprintf(stderr, ERR_FILE_PREMATURE_EOF_ARG("address", line));
        return ERROR;
    }

    if (sepChar == '\n') {
        // Fehler beim Parsen: Vorzeitiges Ende der Zeile
        (void) fprintf(stderr, ERR_FILE_PREMATURE_NEW_LINE("address", line));
        return ERROR;
    }

    if (sepChar != ',') {
        // Fehler beim Parsen: Ungültiges Argument
        (void) fprintf(stderr, ERR_FILE_INVALID_ARG_RW(line));
        return ERROR;
    }

    // mode zurückgeben

    if (modeChar == 'r' || modeChar == 'R') {
        return OK_READ;
    }

    return OK_WRITE;

}

int getAddressArg(FILE *file, uint32_t *res, size_t line) {

    // Initialisiere array fürs lesen

    char *address_string = malloc(sizeof(char) * MAX_ARG_LENGTH);

    if (address_string == NULL) {
        // Fehler beim allokieren von Speicher
        (void) fprintf(stderr, ERR_GENERAL_MEMORY_ALLOCATION_ERROR);
        return 1;
    }

    // Einlesen bis ',' '\n' oder 11 Zeichen

    for (int i = 0; i < MAX_ARG_LENGTH; i++) {

        int current = getNextChar(file);

        if (feof(file)) {
            // Fehler beim Parsen: Vorzeitiges Ende der Datei
            free(address_string);
            (void) fprintf(stderr, ERR_FILE_PREMATURE_EOF_ARG("address", line));
            return 1;
        }

        if (current == ' ') {
            // Fehler beim Parsen: Leerzeichen nicht erlaubt
            free(address_string);
            (void) fprintf(stderr, ERR_FILE_WHITE_SPACE(line));
            return 1;
        }

        if (current == '\n') {
            // Fehler beim Parsen: Vorzeitiges Ende der Zeile
            free(address_string);
            (void) fprintf(stderr, ERR_FILE_PREMATURE_NEW_LINE("address", line));
            return 1;
        }

        if (i >= MAX_ARG_LENGTH - 1 && current != ',') {
            // Fehler beim Parsen: Ungültiges Argument
            free(address_string);
            (void) fprintf(stderr, ERR_FILE_INVALID_ARG_ADDR(line));
            return 1;
        }

        if (current == ',') {

            // wenn leeres Argument
            if (i == 0) {
                // Fehler beim Parsen: Ungültiges Argument
                free(address_string);
                (void) fprintf(stderr, ERR_FILE_INVALID_ARG_ADDR(line));
                return 1;
            }

            // Null-Byte anhängen
            address_string[i] = 0;
            break;

        }

        // Aktuelles Zeichen anhängen
        address_string[i] = (char) current;

    }

    // string in int konvertieren

    long address_int = 0;
    errno = 0;
    char *end = NULL;

    if (address_string[0] == '0' && address_string[1] == 'x') {
        address_int = strtol(address_string, &end, HEX_BASE);
    } else {
        address_int = strtol(address_string, &end, DEC_BASE);
    }

    if (errno || *end != '\0') {
        // Fehler beim Parsen: Ungültiges Argument
        free(address_string);
        (void) fprintf(stderr, ERR_FILE_INVALID_ARG_ADDR(line));
        return 1;
    }

    if (address_int > MAX_ARG_VALUE) {
        // Fehler beim Parsen: Ungültiges Argument
        free(address_string);
        (void) fprintf(stderr, ERR_FILE_INVALID_ARG_ADDR(line));
        return 1;
    }

    // Adresse zurückgeben
    *res = (uint32_t) address_int;
    free(address_string);
    return 0;

}

int getDataArg(FILE *file, uint32_t *res, RetCode mode, size_t line) {


    // Wenn im Lesemodus, muss leeres Argument
    if (mode == OK_READ) {

        int current = getNextChar(file);

        if (feof(file)) {
            // Fehler beim Parsen: Vorzeitiges Ende der Datei
            (void) fprintf(stderr, ERR_FILE_PREMATURE_EOF_NL(line));
            return 1;
        }

        if (current == ' ') {
            // Fehler beim Parsen: Leerzeichen nicht erlaubt
            (void) fprintf(stderr, ERR_FILE_WHITE_SPACE(line));
            return 1;
        }

        if (current == ',') {
            // Fehler beim Parsen: Zu viele Argumente
            (void) fprintf(stderr, ERR_FILE_TOO_MANY_ARGS(line));
            return 1;
        }

        if (current != '\n') {
            // Fehler beim Parsen: Ungültiges Argument
            (void) fprintf(stderr, ERR_FILE_INVALID_ARG_DATA_READ(line));
            return 1;
        }

        return 0;

    }

    // Initialisiere array fürs lesen

    char *data_string = malloc(sizeof(char) * MAX_ARG_LENGTH);

    if (data_string == NULL) {
        // Fehler beim allokieren von Speicher
        free(data_string);
        (void) fprintf(stderr, ERR_GENERAL_MEMORY_ALLOCATION_ERROR);
        return 1;
    }

    // Einlesen bis ',' '\n' oder 11 Zeichen

    for (int i = 0; i < MAX_ARG_LENGTH; i++) {

        int current = getNextChar(file);

        if (feof(file)) {
            // Fehler beim Parsen: Vorzeitiges Ende der Datei
            free(data_string);
            (void) fprintf(stderr, ERR_FILE_PREMATURE_EOF_NL(line));
            return 1;
        }

        if (current == ' ') {
            // Fehler beim Parsen: Leerzeichen nicht erlaubt
            free(data_string);
            (void) fprintf(stderr, ERR_FILE_WHITE_SPACE(line));
            return 1;
        }

        if (current == ',') {
            // Fehler beim Parsen: Zu viele Argumente
            free(data_string);
            (void) fprintf(stderr, ERR_FILE_TOO_MANY_ARGS(line));
            return 1;
        }

        if (i == MAX_ARG_LENGTH - 1 && current != '\n') {
            // Fehler beim Parsen: Ungültiges Argument
            free(data_string);
            (void) fprintf(stderr, ERR_FILE_INVALID_ARG_DATA_WRITE(line));
            return 1;
        }

        if (current == '\n') {

            // wenn leers Argument
            if (i == 0) {
                // Fehler beim Parsen: Ungültiges Argument
                free(data_string);
                (void) fprintf(stderr, ERR_FILE_INVALID_ARG_DATA_WRITE(line));
                return 1;
            }

            // Null-Byte anhängen
            data_string[i] = '\0';
            break;

        }

        // Aktuelles Zeichen anhängen
        data_string[i] = (char) current;

    }

    // string in int konvertieren

    long data_int = 0;
    errno = 0;
    char *end = NULL;

    if (data_string[0] == '0' && data_string[1] == 'x') {
        data_int = strtol(data_string, &end, HEX_BASE);
    } else {
        data_int = strtol(data_string, &end, DEC_BASE);
    }

    if (errno || *end != '\0') {
        // Fehler beim Parsen: Ungültiges Argument
        free(data_string);
        (void) fprintf(stderr, ERR_FILE_INVALID_ARG_DATA_WRITE(line));
        return 1;
    }

    if (data_int > MAX_ARG_VALUE) {
        // Fehler beim Parsen: Ungültiges Argument
        free(data_string);
        (void) fprintf(stderr, ERR_FILE_INVALID_ARG_DATA_WRITE(line));
        return 1;
    }

    // Adresse zurückgeben
    *res = (uint32_t) data_int;
    free(data_string);
    return 0;

}
