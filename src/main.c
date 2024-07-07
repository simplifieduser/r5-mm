#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include "shared.h"
#include "file_parser.h"

extern Result run_simulation(int cycles, unsigned tlbSize, unsigned tlbsLatency, unsigned blocksize, unsigned v2bBlockOffset, unsigned memoryLatency, size_t numRequests, Request *requests, const char *tracefile);

int main(int argc, char *argv[]) {
    int cycles = 10000;
    unsigned int tlbSize = 64;
    unsigned int tlbLatency = 1;
    unsigned int blocksize = 4;
    unsigned int v2bBlockOffset = 4;
    unsigned int memoryLatency = 100;
    const char *tracefile = NULL;
    const char *inputfile = NULL;
    /*
     * die Werte von v2bBlockOffset und cycles wurden beliebig gewählt, da es in der Realität keinen v2bBlockOffset gibt
     * alle anderen Werte stammen von: Patterson, D. A., Hennessy, J. L. (). Computer Organization and Design: The Hardware/Software Interface. (4th ed.). Morgan Kaufman. Seite 503.
    */
    int opt = 0;
    static struct option long_options[] = {
            {"cycles",           required_argument, 0, 'c'},
            {"blocksize",        required_argument, 0, 'b'},
            {"v2b-block-offset", required_argument, 0, 'o'},
            {"tlb-size",         required_argument, 0, 's'},
            {"tlb-latency",      required_argument, 0, 't'},
            {"memory-latency",   required_argument, 0, 'm'},
            {"tf",               required_argument, 0, 'f'},
            {"help",             no_argument,       0, 'h'},
            {0, 0,                                  0, 0}
    };
    opterr = 0; // Stellt die Fehlermeldungen von getopt_long still, um eigene Fehlermeldungen auszugeben

    // Durchsucht die übergebenen Optionen ob Hilfe gedruckt werden soll
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-h", 2) == 0 || strncmp(argv[i], "--help", 6) == 0) {
            (void) fprintf(stderr, HELP_MSG"\n");
            return EXIT_SUCCESS;
        }
    }

    // Einlesen der Optionen/Argumente
    while ((opt = getopt_long(argc, argv, ":c:b:o:s:t:m:f:h", long_options, NULL)) != -1) {
        switch (opt) {
            case 'c': {
                char *endOfPointer = NULL;
                errno = 0;
                long tmp = strtol(optarg, &endOfPointer, 10);
                if (errno != 0 || *endOfPointer != '\0' || tmp > INT32_MAX || tmp < 0) {
                    (void) fprintf(stderr, ERR_ILLEGAL_ARGUMENT_CYCLES"\n");
                    return EXIT_FAILURE;
                }
                cycles = (int) tmp;
                break;
            }
            case 'b': {
                char *endOfPointer = NULL;
                errno = 0;
                long tmp = strtol(optarg, &endOfPointer, 10);
                if (errno != 0 || *endOfPointer != '\0' || tmp > UINT32_MAX || tmp <= 0) {
                    (void) fprintf(stderr, ERR_ILLEGAL_ARGUMENT_BLOCKSIZE"\n");
                    return EXIT_FAILURE;
                }
                blocksize = (unsigned) tmp;
                break;
            }
            case 'o': {
                char *endOfPointer = NULL;
                errno = 0;
                long tmp = strtol(optarg, &endOfPointer, 10);
                if (errno != 0 || *endOfPointer != '\0' || tmp > UINT32_MAX || tmp < 0) {
                    (void) fprintf(stderr, ERR_ILLEGAL_ARGUMENT_V2B_BLOCK_OFFSET"\n");
                    return EXIT_FAILURE;
                }
                v2bBlockOffset = (unsigned) tmp;
                break;
            }
            case 's': {
                char *endOfPointer = NULL;
                errno = 0;
                long tmp = strtol(optarg, &endOfPointer, 10);
                if (errno != 0 || *endOfPointer != '\0' || tmp > UINT32_MAX || tmp < 0) {
                    (void) fprintf(stderr, ERR_ILLEGAL_ARGUMENT_TLB_SIZE"\n");
                    return EXIT_FAILURE;
                }
                tlbSize = (unsigned) tmp;
                break;
            }
            case 't': {
                char *endOfPointer = NULL;
                errno = 0;
                long tmp = strtol(optarg, &endOfPointer, 10);
                if (errno != 0 || *endOfPointer != '\0' || tmp > UINT32_MAX || tmp < 0) {
                    (void) fprintf(stderr, ERR_ILLEGAL_ARGUMENT_TLB_LATENCY"\n");
                    return EXIT_FAILURE;
                }
                tlbLatency = (unsigned) tmp;
                break;
            }
            case 'm': {
                char *endOfPointer = NULL;
                errno = 0;
                long tmp = strtol(optarg, &endOfPointer, 10);
                if (errno != 0 || *endOfPointer != '\0' || tmp > UINT32_MAX || tmp < 0) {
                    (void) fprintf(stderr, ERR_ILLEGAL_ARGUMENT_MEMORY_LATENCY"\n");
                    return EXIT_FAILURE;
                }
                memoryLatency = (unsigned) tmp;
                break;
            }
            case 'f': {
                tracefile = optarg;
                FILE *file = NULL; // Erstelle eine Datei mit den Namen der künftigen Tracefile
                file = fopen(tracefile, "we");
                if (file == NULL) {
                    (void) fprintf(stderr, ERR_ILLEGAL_ARGUMENT_TRACEFILE"\n");
                    return EXIT_FAILURE;
                }
                if (fclose(file) != 0) {
                    (void) fprintf(stderr, ERR_GENERAL_UNKNOWN);
                    return EXIT_FAILURE;
                }
                break;
            }
            case 'h': {
                (void) fprintf(stderr, HELP_MSG"\n");
                return EXIT_SUCCESS;
            }
                // Fehlerbehandlung
            case '?': {
                if (optopt) {
                    (void) fprintf(stderr, ERR_UNKNOWN_OPTION "'-%c'\n"MSG_HINT"\n", optopt);
                    return EXIT_FAILURE;
                }
                (void) fprintf(stderr, ERR_UNKNOWN_OPTION "'%s'\n"MSG_HINT"\n", argv[optind - 1]);
                return EXIT_FAILURE;
            }
            case ':': {
                (void) fprintf(stderr, ERR_NO_REQUIRED_ARGUMENT"'%s'\n"MSG_HINT"\n", argv[optind - 1]);
                return EXIT_FAILURE;
            }
            default:
                (void) fprintf(stderr, ERR_UNKNOWN_OPTION "'%s'.\n", argv[optind - 1]);
                return EXIT_FAILURE;
        }
    }


    if (optind < argc) {

        inputfile = argv[optind]; // Mögliche Fehlerbehandlung folgt beim Einlesen der Datei

    } else {
        (void) fprintf(stderr, ERR_NO_FILE_INPUT"\n"MSG_HINT"\n");
        return EXIT_FAILURE;
    }
    if (optind < argc - 1) {
        (void) fprintf(stderr, ERR_TOO_MANY_OPTION"\n"MSG_HINT"\n");
        return EXIT_FAILURE;
    }

    // Lese Requests aus Datei

    Request *requests = NULL;
    size_t requestCount = 0;
    int status = parseFile(inputfile, &requestCount, &requests);

    if (status < 0) {
        return EXIT_FAILURE;
    }

    // Führe Simulation aus & beende mit Erfolg

    (void) run_simulation(cycles, tlbSize, tlbLatency, blocksize, v2bBlockOffset, memoryLatency, requestCount, requests, tracefile);

    return EXIT_SUCCESS;
}
