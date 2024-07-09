#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include "shared.h"
#include "file_parser.h"

extern Result run_simulation(int cycles, unsigned tlbSize, unsigned tlbsLatency, unsigned blocksize, unsigned v2bBlockOffset, unsigned memoryLatency, size_t numRequests, Request *requests, const char *tracefile);

int main(int argc, char *argv[]) {
    int cycles = 1000000;
    unsigned int tlbSize = 64;
    unsigned int tlbLatency = 1;
    unsigned int blocksize = 4096;
    unsigned int v2bBlockOffset = 4;
    unsigned int memoryLatency = 100;
    const char *tracefile = NULL;
    const char *inputfile = NULL;
    bool cycles_bool = 0;
    bool tlbSize_bool = 0;
    bool tlbLatency_bool = 0;
    bool blocksize_bool = 0;
    bool v2bBlockOffset_bool = 0;
    bool memoryLatency_bool = 0;
    bool tracefile_bool = 0;
    /*
     * die Werte von v2bBlockOffset und cycles wurden beliebig gewählt, da es in der Realität keinen v2bBlockOffset gibt
     * alle anderen Werte stammen von: Patterson, D. A., Hennessy, J. L. (). Computer Organization and Design: The Hardware/Software Interface. (4th ed.). Morgan Kaufman. Seite 503.
    */
    int opt = 0; // Quelle: man getopt_long
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
    
    // Stellt die Fehlermeldungen von getopt_long still, um eigene Fehlermeldungen auszugeben
    opterr = 0; 

    // Einlesen der Optionen/Argumente
    while (1) {
        // nächste Option, erstes ':' ermöglicht präzisere Fehlermeldung
        opt = getopt_long(argc, argv, ":c:b:o:s:t:m:f:h", long_options, NULL);
        
        // keine weiteren Optionen
        if (opt ==-1) break;
        
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
            if(tracefile_bool == 1) {
                (void) fprintf(stderr, ERR_AlREADY_SET("tracefile"));
                return EXIT_FAILURE;
            }
            tracefile_bool = 1;
            FILE *file = NULL; 
            // Erstellen der Datei, zur Überprüfung möglicher unzulässiger Dateinamen mit angehängtem '.vcd'
            tracefile = optarg;

            char tracefile_tmp[strlen(optarg)+5];
            //TODO CHECK IF NULL
            snprintf(tracefile_tmp, strlen(optarg)+5,"%s.vcd",tracefile); // Quelle: https://stackoverflow.com/questions/11836064/c-creating-new-file-extensions-based-on-a-filename

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
        
        // Umwandlung der Eingabewerte für die folgenden Optionen 
        char *endptr = NULL;
        errno = 0;
        long value = strtol(optarg, &endptr, 10); // Quelle: man strtol
        switch (opt) {
            // cycles: -c / --cycles
            case 'c': {
                if(cycles_bool == 1) {
                    (void) fprintf(stderr, ERR_AlREADY_SET("cycles"));
                    return EXIT_FAILURE;
                }
                cycles_bool = 1;

                if (errno != 0 || *endptr != '\0' || value > INT32_MAX || value < 0) {
                    (void) fprintf(stderr, ERR_ILLEGAL_ARGUMENT("cycles (-c/--cycles)",0,INT32_MAX));
                    return EXIT_FAILURE;
                }
                cycles = (int) value;
                break;
            }

            // blocksize: -b / --blocksize
            case 'b': {
                if(blocksize_bool == 1) {
                    (void) fprintf(stderr, ERR_AlREADY_SET("blocksize"));
                    return EXIT_FAILURE;
                }
                blocksize_bool = 1;

                if (errno != 0 || *endptr != '\0' || value > UINT32_MAX || value <= 0) {
                    (void) fprintf(stderr, ERR_ILLEGAL_ARGUMENT("blocksize (-b/--blocksize)",1,UINT32_MAX));
                    return EXIT_FAILURE;
                }
                blocksize = (unsigned) value;
                break;
            }

            // v2bBlockOffset: -o / --v2bBlockOffset
            case 'o': {
                if(v2bBlockOffset_bool == 1) {
                    (void) fprintf(stderr, ERR_AlREADY_SET("v2bBlockOffset"));
                    return EXIT_FAILURE;
                }
                v2bBlockOffset_bool = 1;

                if (errno != 0 || *endptr != '\0' || value > UINT32_MAX || value < 0) {
                    (void) fprintf(stderr, ERR_ILLEGAL_ARGUMENT("v2b-block-offset (-o/--v2b-block-offset",0,UINT32_MAX));
                    return EXIT_FAILURE;
                }
                v2bBlockOffset = (unsigned) value;
                break;
            }

            // tlb-size: -s / --tlb-size
            case 's': {
                if(tlbSize_bool == 1) {
                    (void) fprintf(stderr, ERR_AlREADY_SET("tlb-size"));
                    return EXIT_FAILURE;
                }
                tlbSize_bool = 1;

                if (errno != 0 || *endptr != '\0' || value > UINT32_MAX || value < 0) {
                    (void) fprintf(stderr, ERR_ILLEGAL_ARGUMENT("tlb-size (-s/--tlb-size)",0,UINT32_MAX));
                    return EXIT_FAILURE;
                }
                tlbSize = (unsigned) value;
                break;
            }

            // tlb-latency: -t / --tlb-latency
            case 't': {
                if(tlbLatency_bool == 1) {
                    (void) fprintf(stderr, ERR_AlREADY_SET("tlb-latency"));
                    return EXIT_FAILURE;
                }
                tlbLatency_bool = 1;

                if (errno != 0 || *endptr != '\0' || value > UINT32_MAX || value < 0) {
                    (void) fprintf(stderr, ERR_ILLEGAL_ARGUMENT("tlb-latency (-t/--tlb-latency)",0,UINT32_MAX));
                    return EXIT_FAILURE;
                }
                tlbLatency = (unsigned) value;
                break;
            }

            //memory-latency -m / --memory-latency
            case 'm': {
                if(memoryLatency_bool == 1) {
                    (void) fprintf(stderr, ERR_AlREADY_SET("memoryLatency"));
                    return EXIT_FAILURE;
                }
                memoryLatency_bool = 1;

                if (errno != 0 || *endptr != '\0' || value > UINT32_MAX || value < 0) {
                    (void) fprintf(stderr, ERR_ILLEGAL_ARGUMENT("memory-latency (-m/--memory-latency)",0,UINT32_MAX));
                    return EXIT_FAILURE;
                }
                memoryLatency = (unsigned) value;
                break;
            }

            default: {
                (void) fprintf(stderr,ERR_GENERAL_UNKNOWN);
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

    Result result = run_simulation(cycles, tlbSize, tlbLatency, blocksize, v2bBlockOffset, memoryLatency, requestCount, requests, tracefile);

    // ------------ TESTING CODE ------------
#ifdef TEST_BUILD

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

#endif
    // ------------ TESTING CODE ------------

    // Speicher freigeben & Programm beenden

    free(requests);

    return EXIT_SUCCESS;
}
