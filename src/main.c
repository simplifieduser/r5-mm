#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>
#include "messages.h"
#include "file_parser.h"

int main(int argc, char*argv[]) {
    int cycles = 10000;
    unsigned int tlbSize = 64;
    unsigned int tlbLatency = 1;
    unsigned int blocksize = 4;
    unsigned int v2bBlockOffset = 4;
    unsigned int memoryLatency = 100;
    const char* tracefile = NULL;
    const char* inputfile;
    /*
     * die Werte von v2bBlockOffset und cycles wurden beliebig gewählt, da es in der Realität keinen v2bBlockOffset gibt
     * alle anderen Werte stammen von: Patterson, D. A., Hennessy, J. L. (). Computer Organization and Design: The Hardware/Software Interface. (4th ed.). Morgan Kaufman. Seite 503.
    */
    int opt;
    static struct option long_options[] = {
            {"cycles",required_argument,0,'c'},
            {"blocksize",required_argument,0,'b'},
            {"v2b-block-offset",required_argument,0,'o'},
            {"tlb-size",required_argument,0,'s'},
            {"tlb-latency", required_argument,0,'t'},
            {"memory-latency", required_argument,0,'m'},
            {"tf",required_argument,0,'f'},
            {"help", no_argument,0,'h'},
            {0,0,0,0}
    };
    opterr = 0; // Stellt die Fehlermeldungen von getopt_long still, um eigene Fehlermeldungen auszugeben

    // Durchsucht die übergebenen Optionen ob Hilfe gedruckt werden soll
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-h",2) == 0 || strncmp(argv[i], "--help",6) == 0) {
            fprintf(stderr,HELP_MSG"\n");
            exit(EXIT_SUCCESS);
        }
    }

    // Einlesen der Optionen/Argumente
    while ((opt = getopt_long(argc, argv, ":c:b:o:s:t:m:f:h", long_options, NULL))!= -1){
        switch (opt){
            case 'c':{
                char *endOfPointer;
                errno = 0;
                long tmp = strtol(optarg,&endOfPointer,10);
                if(errno != 0|| *endOfPointer != '\0'|| tmp > INT32_MAX || tmp<0){
                    fprintf(stderr,ILLEGAL_ARGUMENT_CYCLES"\n");
                    exit(EXIT_FAILURE);
                } else {
                    cycles = (int) tmp;
                }
                break;
            }
            case 'b':{
                char *endOfPointer;
                errno = 0;
                long tmp = strtol(optarg,&endOfPointer,10);
                if(errno != 0|| *endOfPointer != '\0'|| tmp > UINT32_MAX || tmp<=0){
                    fprintf(stderr,ILLEGAL_ARGUMENT_BLOCKSIZE"\n");
                    exit(EXIT_FAILURE);
                } else {
                    blocksize = (unsigned) tmp;
                }
                break;
            }
            case 'o':{
                char *endOfPointer;
                errno = 0;
                long tmp = strtol(optarg,&endOfPointer,10);
                if(errno != 0|| *endOfPointer != '\0'|| tmp > UINT32_MAX || tmp<0){
                    fprintf(stderr,ILLEGAL_ARGUMENT_V2B_BLOCK_OFFSET"\n");
                    exit(EXIT_FAILURE);
                } else {
                    v2bBlockOffset = (unsigned) tmp;
                }
                break;
            }
            case 's':{
                char *endOfPointer;
                errno = 0;
                long tmp = strtol(optarg,&endOfPointer,10);
                if(errno != 0|| *endOfPointer != '\0'|| tmp > UINT32_MAX || tmp<0){
                    fprintf(stderr,ILLEGAL_ARGUMENT_TLB_SIZE"\n");
                    exit(EXIT_FAILURE);
                } else {
                    tlbSize = (unsigned) tmp;
                }
                break;
            }
            case 't':{
                char *endOfPointer;
                errno = 0;
                long tmp = strtol(optarg,&endOfPointer,10);
                if(errno != 0|| *endOfPointer != '\0'|| tmp > UINT32_MAX || tmp<0){
                    fprintf(stderr,ILLEGAL_ARGUMENT_TLB_LATENCY"\n");
                    exit(EXIT_FAILURE);
                } else {
                    tlbLatency = (unsigned) tmp;
                }
                break;
            }
            case 'm':{
                char *endOfPointer;
                errno = 0;
                long tmp = strtol(optarg,&endOfPointer,10);
                if(errno != 0|| *endOfPointer != '\0'|| tmp > UINT32_MAX || tmp<0){
                    fprintf(stderr,ILLEGAL_ARGUMENT_MEMORY_LATENCY"\n");
                    exit(EXIT_FAILURE);
                } else {
                    memoryLatency = (unsigned) tmp;
                }
                break;
            }
            case 'f':{
                tracefile = optarg;
                FILE *file; // Erstelle eine Datei mit den Namen der künftigen Tracefile
                file = fopen(tracefile,"w");
                if(file == NULL) {
                    fprintf(stderr,ILLEGAL_ARGUMENT_TRACEFILE"\n");
                    exit(EXIT_FAILURE);
                }
                fclose(file);
                break;
            }
            case 'h':{
                fprintf(stderr,HELP_MSG"\n");
                exit(EXIT_SUCCESS);
            }
            // Fehlerbehandlung
            case '?':{
                if(optopt) {
                    fprintf(stderr, UNKNOWN_OPTION "'-%c'\n"HINT"\n", optopt);
                    exit(EXIT_FAILURE);
                }else {
                    fprintf(stderr, UNKNOWN_OPTION "'%s'\n"HINT"\n", argv[optind - 1]);
                    exit(EXIT_FAILURE);
                }
            }
            case ':':{

                fprintf(stderr, NO_REQUIRED_ARGUMENT"'%s'\n"HINT"\n", argv[optind - 1]);
                exit(EXIT_FAILURE);
            }
            default:
                fprintf(stderr, UNKNOWN_OPTION "'%s'.\n", argv[optind - 1]);
                exit(EXIT_FAILURE);
        }
    }


    if(optind < argc) {

        inputfile = argv[optind];// Mögliche Fehlerbehandlung folgt beim Einlesen der Datei

    } else {
        fprintf(stderr,NO_FILE_INPUT"\n"HINT"\n");
        exit(EXIT_FAILURE);
    }
    if(optind < argc - 1) {
        fprintf(stderr,TOO_MANY_OPTION"\n"HINT"\n");
        exit(EXIT_FAILURE);
    }

    // Read input file

    Request *requests = NULL;
    int requestCount = parseFile(inputfile, &requests);

    if (requestCount < 0) {
        exit(EXIT_FAILURE);
    }

    //TODO: Implement the simulation

    // Zum Testen der Eingaben
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

    for (int i = 0; i < requestCount; ++i) {
        printf("%d: %d %u %u\n", i, requests[i].we, requests[i].addr, requests[i].data);
    }

    exit(EXIT_SUCCESS);
}


