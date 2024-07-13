#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include "shared.h"
#include "file_parser.h"

extern Result
run_simulation(int cycles, unsigned tlbSize, unsigned tlbsLatency, unsigned blocksize, unsigned v2bBlockOffset,
               unsigned memoryLatency, size_t numRequests, Request *requests, const char *tracefile);
enum RetCode {
    OK, ERR
} typedef RetCode;

// Umwandlung der Eingabe String zu Integer Werten mit Fehlerbehandlung
RetCode inputConversion (int *booleanValue,char errorMSG[], char inputString[], uint32_t lowerBound, uint32_t upperBound, uint32_t *value);

// Überprüfung, ob eine Option bereits gesetzt wurde
RetCode alreadySetCheck(int *booleanValue, char errorMSG[]);

// Ausgabe der gesetzten und ermittelten Werte
void printDebug(Request* requests, size_t requestCount, Result result);

// Globale variablen für das Parameter Parsing
unsigned cycles = 1000000;
unsigned tlbSize = 64;
unsigned tlbLatency = 1;
unsigned blocksize = 4096;
unsigned v2bBlockOffset = 4;
unsigned memoryLatency = 100;
const char *tracefile = NULL;
const char *inputfile = NULL;
int cycles_bool = 0, tlbSize_bool = 0, tlbLatency_bool = 0, blocksize_bool = 0, v2bBlockOffset_bool = 0, memoryLatency_bool = 0, tracefile_bool = 0;

/*
 * die Werte von v2bBlockOffset und cycles wurden beliebig gewählt, da diese nur für die Simulation relevant sind und bei einem echten TLB nicht existieren
 * alle anderen Werte stammen von: Patterson, D. A., Hennessy, J. L. (). Computer Organization and Design: The Hardware/Software Interface. (4th ed.). Morgan Kaufman. Seite 503(ff.).
*/
int main(int argc, char *argv[]) {

    int quickStart_bool = 0;
    // vgl. getopt_long man getopt_long, Grundlagenpraktikum Rechnerarchitektur SS24, Aufgabe: Nutzereingaben
    static struct option long_options[] = {
            {"help",             no_argument,       0, 'h'},
            {"tf",               required_argument, 0, 'f'},
            {"cycles",           required_argument, 0, 'c'},
            {"blocksize",        required_argument, 0, 'b'},
            {"v2b-block-offset", required_argument, 0, 'o'},
            {"tlb-size",         required_argument, 0, 's'},
            {"tlb-latency",      required_argument, 0, 't'},
            {"memory-latency",   required_argument, 0, 'm'},
            {"quickStart",       no_argument,       0, 'q'},
            {0, 0,                                  0, 0}
    };
    opterr = 0; // Stellt die Fehlermeldungen von getopt_long still, um eigene Fehlermeldungen auszugeben
    int opt;

    // Einlesen der Optionen/Argumente
    while (1) {

        // nächste Option, erstes ':' ermöglicht präzisere Fehlermeldung
        opt = getopt_long(argc, argv, ":hqf:c:b:o:s:t:m:", long_options, NULL);

        // keine weiteren Optionen
        if (opt == -1) break;

        // -h / --help
        if (opt == 'h') {
            (void) fprintf(stderr, HELP_MSG);
            return EXIT_SUCCESS;
        }

        // quickStart, setzt die Werte aus dem Beispiel unter der HELP Nachricht, beachte, dass alle bisherige Werte überschrieben werden und künftige Argumente nicht berücksichtigt werden
        if (opt == 'q') {
            quickStart_bool = 1;
            cycles = 2000;
            tlbSize = 16;
            tlbLatency = 2;
            blocksize = 16;
            v2bBlockOffset = 4;
            memoryLatency = 100;
            tracefile = "tracefile";
            inputfile = "examples/kurzeEingabedatei_valid.csv";
            break;
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
            if (alreadySetCheck(&tracefile_bool,tracefile_str) == ERR) return EXIT_FAILURE;

            // Erstellen der Datei, zur Überprüfung möglicher unzulässiger Dateinamen mit angehängtem '.vcd'
            // vgl.: https://stackoverflow.com/questions/11836064/c-creating-new-file-extensions-based-on-a-filename
            FILE *file = NULL;
            tracefile = optarg;
            char tracefile_tmp[strlen(optarg) + 5];

            // NOLINTNEXTLINE: snprintf_s steht in dieser Umgebung nicht zur Verfügung
            snprintf(tracefile_tmp, strlen(optarg) + 5, "%s.vcd", tracefile);

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

        // Alle diese Optionen benötigen einen Integer als Argument
        switch (opt) {
            // cycles: -c / --cycles
            case 'c': {
                if ( inputConversion(&cycles_bool,cycle_str,optarg,0,INT32_MAX, &cycles) == ERR) return EXIT_FAILURE;
                break;
            }

            // blocksize: -b / --blocksize
            case 'b': {
                if ( inputConversion(&blocksize_bool,blocksize_str,optarg,1,UINT32_MAX,&blocksize) == ERR) return EXIT_FAILURE;
                break;
            }

            // v2bBlockOffset: -o / --v2bBlockOffset
            case 'o': {
                if ( inputConversion(&v2bBlockOffset_bool,v2b_block_offset_str,optarg,0,UINT32_MAX, &v2bBlockOffset) == ERR) return EXIT_FAILURE;
                break;
            }

            // tlb-size: -s / --tlb-size
            case 's': {
                if (inputConversion(&tlbSize_bool,tlb_size_str,optarg,0,UINT32_MAX, &tlbSize) == ERR ) return EXIT_FAILURE;
                break;
            }

            // tlb-latency: -t / --tlb-latency
            case 't': {
                if ( inputConversion(&tlbLatency_bool,tlb_latency_str,optarg,0,UINT32_MAX, & tlbLatency) == ERR) return EXIT_FAILURE;
                break;
            }

            //memory-latency -m / --memory-latency
            case 'm': {
                if ( inputConversion(&memoryLatency_bool,memory_latency_str,optarg,0,UINT32_MAX, &memoryLatency) == ERR) return EXIT_FAILURE;
                break;
            }

            default: {
                (void) fprintf(stderr, ERR_GENERAL_UNKNOWN);
                return EXIT_FAILURE;
            }

        }
    }

    // inputfile
    // vgl. https://stackoverflow.com/questions/18079340/using-getopt-in-c-with-non-option-arguments
    if (quickStart_bool == 0) {
        if (optind == argc) {
            (void) fprintf(stderr, ERR_NO_FILE_INPUT);
            return EXIT_FAILURE;
        } else if (optind < argc - 1) {
            (void) fprintf(stderr, ERR_TOO_MANY_OPTION);
            return EXIT_FAILURE;
        } else {
            inputfile = argv[optind];
        }
    }


    // Lese Requests aus Datei

    Request *requests = NULL;
    size_t requestCount = 0;
    int status = parseFile(inputfile, &requestCount, &requests);

    if (status < 0) {
        return EXIT_FAILURE;
    }

    // Simulation starten

#ifndef DEBUG_BUILD
    (void) run_simulation(cycles, tlbSize, tlbLatency, blocksize, v2bBlockOffset, memoryLatency, requestCount, requests, tracefile);
#else
    Result result = run_simulation((int) cycles, tlbSize, tlbLatency, blocksize, v2bBlockOffset, memoryLatency, requestCount, requests, tracefile);
    printDebug(requests, requestCount, result);
#endif

    // Speicher freigeben & Programm beenden

    free(requests);

    return EXIT_SUCCESS;
}



// ------------ DEBUG CODE ------------

void createDebugMSG_options () {

    printf("'cycles (-c/--cycles)'                      hat den Wert: %u\n",cycles);
    printf("'blocksize (-b/--blocksize)'                hat den Wert: %u\n",blocksize);
    printf("'v2b-block-offset (-o/--v2b-block-offset)'  hat den Wert: %u\n",v2bBlockOffset);
    printf("'tlb-size (-s/--tlb-size)'                  hat den Wert: %u\n",tlbSize);
    printf("'tlb-latency (-t/--tlb-latency)'            hat den Wert: %u\n",tlbLatency);
    printf("'memory-latency (-m/--memory-latency)'      hat den Wert: %u\n",memoryLatency);
    printf("'tracefile (-f/--tf)'                       hat den Wert: %s\n",tracefile);
    printf("'Eingabedatei'                              hat den Wert: %s\n",inputfile);
}

void createDebugMSG_sim (unsigned  sim_cycles, unsigned sim_misses, unsigned sim_hits, unsigned sim_primitive_gate_count) {

    printf("'cycles'                                    hat den Wert: %u\n",sim_cycles);
    printf("'misses'                                    hat den Wert: %u\n",sim_misses);
    printf("'hits'                                      hat den Wert: %u\n",sim_hits);
    printf("'primitive_gate_count'                      hat den Wert: %u\n",sim_primitive_gate_count);
}


void printDebug(Request* requests, size_t requestCount, Result result) {

    printf("\n\n#1 Optionen und ihre jeweiligen Argumente:\n\n");
    createDebugMSG_options();

    printf("\n#2 Übergebene requests, in der From: index, write(1) oder read(0), Adresse, Datum:\n\n");
    for (size_t i = 0; i < requestCount; ++i) {
        printf("%zu: %d %u %u\n", i, requests[i].we, requests[i].addr, requests[i].data);
    }

    printf("\n#3 Durch die Simulation ermittelte Werte:\n\n");
    createDebugMSG_sim(result.cycles,result.misses,result.hits,result.primitive_gate_count);

}

// ------------ DEBUG CODE ------------


RetCode alreadySetCheck(int *booleanValue, char errorMSG[]){
    // überprüfe, ob dieser Wert bereits gesetzt wurde
    if (*booleanValue == 1) {
        (void) fprintf(stderr, ERR_AlREADY_SET(errorMSG));
        return ERR;
    }
    *booleanValue = 1;
    return OK;
}


RetCode inputConversion (int *booleanValue,char errorMSG[], char inputString[], uint32_t lowerBound, uint32_t upperBound, uint32_t *value) {

    // Überprüfen, ob Option schon vorher gesetzt wird
    if ( alreadySetCheck(booleanValue,errorMSG) == ERR) return ERR;

    // vgl. man strtol, Grundlagenpraktikum Rechnerarchitektur SS24, Aufgabe: Nutzereingaben
    char *endptr = NULL;
    errno = 0;
    uint32_t tmp;

    // Umwandlung des Arguments in hex oder dezimal
    if (inputString[0] == '0' && inputString[1] == 'x') {
        tmp = strtol(inputString, &endptr, 16);
    } else {
        tmp = strtol(inputString, &endptr, 10);
    }

    // Fehler bei der Umwandlung
    if(errno || *endptr != '\0') {
        (void) fprintf(stderr, ERR_ILLEGAL_ARGUMENT_CONVERSION(errorMSG));
        return ERR;
    }

    // Nicht im Wertebereich
    if (inputString[0]=='-'||tmp < lowerBound || tmp > upperBound) {
        (void) fprintf(stderr, ERR_ILLEGAL_ARGUMENT(errorMSG, lowerBound, upperBound));
        return ERR;
    }
    *value = tmp;
    return OK;
}
