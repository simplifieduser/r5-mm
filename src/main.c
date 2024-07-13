#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include "shared.h"
#include "file_parser.h"

extern Result
run_simulation(int cycles, unsigned tlbSize, unsigned tlbsLatency, unsigned blocksize, unsigned v2bBlockOffset,
               unsigned memoryLatency, size_t numRequests, Request *requests, const char *tracefile);

uint32_t inputConversion (bool *booleanValue,char errorMSG[], char inputString[], uint32_t lowerBound, uint32_t upperBound);
void alreadySetCheck(bool *booleanValue, char errorMSG[]);
int cycles = 1000000;
unsigned int tlbSize = 64;
unsigned int tlbLatency = 1;
unsigned int blocksize = 4096;
unsigned int v2bBlockOffset = 4;
unsigned int memoryLatency = 100;
const char *tracefile = NULL;
const char *inputfile = NULL;
/*
     * die Werte von v2bBlockOffset und cycles wurden beliebig gewählt, da es in der Realität keinen v2bBlockOffset gibt
     * alle anderen Werte stammen von: Patterson, D. A., Hennessy, J. L. (). Computer Organization and Design: The Hardware/Software Interface. (4th ed.). Morgan Kaufman. Seite 503.
*/

int main(int argc, char *argv[]) {
    static struct option long_options[] = { // Quelle für getopt_long: man getopt_long
            {"help",             no_argument,       0, 'h'},
            {"tf",               required_argument, 0, 'f'},
            {"cycles",           required_argument, 0, 'c'},
            {"blocksize",        required_argument, 0, 'b'},
            {"v2b-block-offset", required_argument, 0, 'o'},
            {"tlb-size",         required_argument, 0, 's'},
            {"tlb-latency",      required_argument, 0, 't'},
            {"memory-latency",   required_argument, 0, 'm'},
            {0, 0,                                  0, 0}
    };

    opterr = 0; // Stellt die Fehlermeldungen von getopt_long still, um eigene Fehlermeldungen auszugeben
    int opt;
    bool cycles_bool = 0, tlbSize_bool = 0, tlbLatency_bool = 0, blocksize_bool = 0, v2bBlockOffset_bool = 0, memoryLatency_bool = 0, tracefile_bool = 0;

    // Einlesen der Optionen/Argumente
    while (1) {

        // nächste Option, erstes ':' ermöglicht präzisere Fehlermeldung
        opt = getopt_long(argc, argv, ":hf:c:b:o:s:t:m:", long_options, NULL);

        // keine weiteren Optionen
        if (opt == -1) break;

        // -h / --help
        if (opt == 'h') {
            (void) fprintf(stderr, HELP_MSG);
            return EXIT_SUCCESS;
        }

        // Fehler: required_argument ohne Argument übergeben
        if (opt == ':') {
            (void) fprintf(stderr, ERR_NO_REQUIRED_ARGUMENT(argv[optind - 1]));
            return EXIT_FAILURE;
        }

        //Fehler: unbekannte Option übergeben
        if (opt == '?') {
            (void) fprintf(stderr, ERR_UNKNOWN_OPTION);
            return EXIT_FAILURE;
        }

        // tracefile: -f / --tf
        if (opt == 'f') {
            alreadySetCheck(&tracefile_bool,tracefile_str);

            // Erstellen der Datei, zur Überprüfung möglicher unzulässiger Dateinamen mit angehängtem '.vcd'
            FILE *file = NULL;
            tracefile = optarg;
            char tracefile_tmp[strlen(optarg) + 5];
            snprintf(tracefile_tmp, strlen(optarg) + 5, "%s.vcd", tracefile);
            // vgl.: https://stackoverflow.com/questions/11836064/c-creating-new-file-extensions-based-on-a-filename

            file = fopen(tracefile_tmp, "we");
            if (file == NULL) {
                (void) fprintf(stderr, ERR_ILLEGAL_ARGUMENT_TRACEFILE);
                return EXIT_FAILURE;
            }
            if (fclose(file) != 0) {
                (void) fprintf(stderr, ERR_GENERAL_UNKNOWN);
                return EXIT_FAILURE;
            }
            // Löschen der Datei, falls es vor der Beendigung des Programs zu Fehlern kommt wird keine leere Datei ausgeben
            remove(tracefile_tmp);
            continue;
        }

        switch (opt) {
            // cycles: -c / --cycles
            case 'c': {
                cycles = (int) inputConversion(&cycles_bool,cycle_str,optarg,0,INT32_MAX);
                break;
            }

            // blocksize: -b / --blocksize
            case 'b': {
                blocksize = (unsigned) inputConversion(&blocksize_bool,blocksize_str,optarg,1,UINT32_MAX);
                break;
            }

            // v2bBlockOffset: -o / --v2bBlockOffset
            case 'o': {
                v2bBlockOffset = (unsigned) inputConversion(&v2bBlockOffset_bool,v2b_block_offset_str,optarg,0,UINT32_MAX);
                break;
            }

            // tlb-size: -s / --tlb-size
            case 's': {
                tlbSize = (unsigned) inputConversion(&tlbSize_bool,tlb_size_str,optarg,0,UINT32_MAX);
                break;
            }

            // tlb-latency: -t / --tlb-latency
            case 't': {
                tlbLatency = (unsigned) inputConversion(&tlbLatency_bool,tlb_latency_str,optarg,0,UINT32_MAX);
                break;
            }

            //memory-latency -m / --memory-latency
            case 'm': {
                memoryLatency = (unsigned) inputConversion(&memoryLatency_bool,memory_latency_str,optarg,0,UINT32_MAX);
                break;
            }

            default: {
                (void) fprintf(stderr, ERR_GENERAL_UNKNOWN);
                return EXIT_FAILURE;
            }

        }
    }

    // Eingabedatei
    if (optind < argc) {
        inputfile = argv[optind]; // Mögliche Fehlerbehandlung folgt beim Einlesen der Datei
    } else {
        (void) fprintf(stderr, ERR_NO_FILE_INPUT);
        return EXIT_FAILURE;
    }
    if (optind < argc - 1) {
        (void) fprintf(stderr, ERR_TOO_MANY_OPTION);
        return EXIT_FAILURE;
    }

    // Lese Requests aus Datei

    Request *requests = NULL;
    size_t requestCount = 0;
    int status = parseFile(inputfile, &requestCount, &requests);

    if (status < 0) {
        return EXIT_FAILURE;
    }

    // Simulation starten

#ifdef TEST_BUILD

    // ------------ TESTING CODE ------------

    Result result = run_simulation(cycles, tlbSize, tlbLatency, blocksize, v2bBlockOffset, memoryLatency, requestCount,
                                   requests, tracefile);

    printf("-\n");

    printf("cycles=%d\n", cycles);
    printf("blocksize=%u\n", blocksize);
    printf("v2bBlockOffset=%u\n", v2bBlockOffset);
    printf("tlbSize=%u\n", tlbSize);
    printf("tlbLatency=%u\n", tlbLatency);
    printf("memoryLatency=%u\n", memoryLatency);
    printf("inputfile=%s\n", inputfile);
    if (tracefile) {
        printf("tracefile=%s\n", tracefile);
    }

    printf("-\n");

    for (size_t i = 0; i < requestCount; ++i) {
        printf("%zu: %d %u %u\n", i, requests[i].we, requests[i].addr, requests[i].data);
    }

    printf("-\n");

    printf("cycles=%zu\n", result.cycles);
    printf("misses=%zu\n", result.misses);
    printf("hits=%zu\n", result.hits);
    printf("primitive_gate_count=%zu\n", result.primitive_gate_count);

    printf("-\n");

    // ------------ TESTING CODE ------------

#else

    (void) run_simulation(cycles, tlbSize, tlbLatency, blocksize, v2bBlockOffset, memoryLatency, requestCount, requests, tracefile);

#endif

    // Speicher freigeben & Programm beenden

    free(requests);

    return EXIT_SUCCESS;
}


void alreadySetCheck(bool *booleanValue, char errorMSG[]){
    if (*booleanValue == 1) {
        (void) fprintf(stderr, ERR_AlREADY_SET(errorMSG));
        exit(EXIT_FAILURE);
    }
    *booleanValue = 1;
}


uint32_t inputConversion (bool *booleanValue,char errorMSG[], char inputString[], uint32_t lowerBound, uint32_t upperBound) {
    alreadySetCheck(booleanValue,errorMSG);

    char *endptr = NULL;
    errno = 0;
    long value;
    if (inputString[0] == '0' && inputString[1] == 'x') {
        value = strtol(inputString, &endptr, 16); // Quelle: man strtol
    } else {
        value = strtol(inputString, &endptr, 10);
    }
    if (errno || *endptr != '\0' || value < lowerBound || value > upperBound) {
        (void) fprintf(stderr, ERR_ILLEGAL_ARGUMENT(errorMSG, lowerBound, upperBound));
        exit(EXIT_FAILURE);
    }
    return value;
}
