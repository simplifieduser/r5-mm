#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdint.h>
#include <errno.h>
#include "messages.h"

int main(int argc, char*argv[]) {
    int cycles = 10;
    unsigned int tlbSize = 4;
    unsigned int tlbLatency = 1;
    unsigned int blocksize = 4;
    unsigned int v2bBlockOffset = 5;
    unsigned int memoryLatency = 120;
    const char* tracefile = NULL;
    const char* inputfile;

    int opt;
    int optional_index = 0;
    static struct option long_options[] = {
            {"cycles",optional_argument,0,'c'},
            {"blocksize",optional_argument,0,'b'},
            {"v2b-block-offset",optional_argument,0,'o'},
            {"tlb-size",optional_argument,0,'s'},
            {"tlb-latency", optional_argument,0,'t'},
            {"memory-latency", optional_argument,0,'m'},
            {"tf=",optional_argument,0,'f'},
            {"help", no_argument,0,'h'},
            {0,0,0,0}
    };
    opterr = 0; //silences errors from optlog, error catching done manually
    //searches if one of the option is -h
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-h",2) == 0 || strncmp(argv[i], "--help",6) == 0) {
            fprintf(stderr,"%s\n",HELP_MSG);
            exit(EXIT_SUCCESS);
        }
    }

    while ((opt = getopt_long(argc, argv, "c::b::o::s::t::m::f:h", long_options, NULL))!= -1){
        if (opt == '?') {
            // Unrecognized option or missing option argument
            if (optopt) {
                fprintf(stderr, UNKNOWN_OPTION "'-%c'.\n", optopt); //unrecognized short options
            } else {
                fprintf(stderr, UNKNOWN_OPTION "'%s'.\n", argv[optind - 1]); //unrecognized short options
            }
            exit(EXIT_FAILURE);
        }
        switch (opt){
            case 'c':
                if (optarg) {
                    char *endOfPointer;
                    errno = 0;
                    long tmp = strtol(optarg,&endOfPointer,10);
                    if(errno != 0|| *endOfPointer != '\0'|| tmp > INT32_MAX || tmp<0){
                        fprintf(stderr,"%s\n",ILLEGAL_ARGUMENT_CYCLES);
                        exit(EXIT_FAILURE);
                    } else {
                        cycles = (int) tmp;
                    }
                }
                break;
            case 'b':{
                if (optarg) {
                    char *endOfPointer;
                    errno = 0;
                    long tmp = strtol(optarg,&endOfPointer,10);
                    if(errno != 0|| *endOfPointer != '\0'|| tmp > UINT32_MAX || tmp<=0){
                        fprintf(stderr,"%s\n",ILLEGAL_ARGUMENT_BLOCKSIZE);
                        exit(EXIT_FAILURE);
                    } else {
                        blocksize = (unsigned) tmp;
                    }
                }
                break;
            }
            case 'o':{
                if (optarg) {
                    char *endOfPointer;
                    errno = 0;
                    long tmp = strtol(optarg,&endOfPointer,10);
                    if(errno != 0|| *endOfPointer != '\0'|| tmp > UINT32_MAX || tmp<0){
                        fprintf(stderr,"%s\n",ILLEGAL_ARGUMENT_V2B_BLOCK_OFFSET);
                        exit(EXIT_FAILURE);
                    } else {
                        v2bBlockOffset = (unsigned) tmp;
                    }
                }
                break;
            }
            case 's':{
                if (optarg) {
                    char *endOfPointer;
                    errno = 0;
                    long tmp = strtol(optarg,&endOfPointer,10);
                    if(errno != 0|| *endOfPointer != '\0'|| tmp > UINT32_MAX || tmp<0){
                        fprintf(stderr,"%s\n",ILLEGAL_ARGUMENT_TLB_SIZE);
                        exit(EXIT_FAILURE);
                    } else {
                        tlbSize = (unsigned) tmp;
                    }
                }
                break;
            }
            case 't':{
                if (optarg) {
                    char *endOfPointer;
                    errno = 0;
                    long tmp = strtol(optarg,&endOfPointer,10);
                    if(errno != 0|| *endOfPointer != '\0'|| tmp > UINT32_MAX || tmp<0){
                        fprintf(stderr,"%s\n",ILLEGAL_ARGUMENT_TLB_LATENCY);
                        exit(EXIT_FAILURE);
                    } else {
                        tlbLatency = (unsigned) tmp;
                    }
                }
                break;
            }
            case 'm':{
                if (optarg) {
                    char *endOfPointer;
                    errno = 0;
                    long tmp = strtol(optarg,&endOfPointer,10);
                    if(errno != 0|| *endOfPointer != '\0'|| tmp > UINT32_MAX || tmp<0){
                        fprintf(stderr,"%s\n",ILLEGAL_ARGUMENT_MEMORY_LATENCY);
                        exit(EXIT_FAILURE);
                    } else {
                        memoryLatency = (unsigned) tmp;
                    }
                }
                break;
            }
            case 'f':{
                if (optarg) {
                    tracefile = optarg;
                }
                break;
            }
            default:
                fprintf(stderr,"%s\n",USAGE_MSG);
                exit(EXIT_FAILURE);
        }
    }


    if(optind <argc) {
        inputfile = argv[optind];
        printf("This is the file %s\n",inputfile);
        //I don't guarantee that it is a file
    } else {
        fprintf(stderr,"%s\n",NO_FILE_INPUT);
        exit(EXIT_FAILURE);
    }
    if(optind <argc - 1) {
        fprintf(stderr,"%s\n",USAGE_TOO_MANY_ARGUMENTS);
        exit(EXIT_FAILURE);
    }

    fprintf(stderr,"cycles: %i, tlbSize: %u, tlbLatency: %u, blocksize: %u, v2b: %u, memoryLatency: %u, tracefileLocation: %s, Inputfilename: %s",cycles,tlbSize,tlbLatency,blocksize,v2bBlockOffset,memoryLatency,tracefile,inputfile);

    fprintf(stderr,"%s\n",successful);
    return 0;
}

//TODO errors are currently wrong given so wirte errors for every one and set them correctly then lets go....
/* TODO fragen ob blocksize = 0 erlaubt war!!!!!!
 *./r5mm --blocksize="3" --blocksize=5 -help name; führt dazu, dass help ausgerufen wird, da -h option verohanden ist und sein Argument elp wäre, ok so eigentlich ja.
 *Theoretisch ist iom optarg :h unnötig, da nach -h, --help gesucht wird davor
 * */


//QUESTIONS:
// WHAT SHOULD HAPPEN WHEN I DO -c WITHOUT INPUT ERROR OR JUST USE DEFAULT
// if tf= is not set then nothing should be created, but aufabenstellung isn#t precise what if i do --tf= and then don't give a name error?
//--tf=<name> gibt name raus, owbwohl das = ja zum namen gehört, eigentlich muss == weil bei cycle muss auc ein = hin, can b fixed with simple ==
//wir müssen alle requierd also wenn man die verwendet dann muss ein argumetn folgen, da sonst leer zweischen denen nicht geht