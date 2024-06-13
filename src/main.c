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
    opterr = 0;
    //Durchsucht die übergebenen Optionen ob sie -h oder --help beinhalten
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-h",2) == 0 || strncmp(argv[i], "--help",6) == 0) {
            fprintf(stderr,"%s\n",HELP_MSG);
            exit(EXIT_SUCCESS);
        }
    }

    while ((opt = getopt_long(argc, argv, "c::b::o::s::t::m::f:h", long_options, &optional_index))!= -1){
        if (opt == '?') {
            // Unrecognized option or missing option argument
            if (optopt) {
                fprintf(stderr, UNKNOWN_OPTION "'-%c'.\n", optopt);
            } else {
                fprintf(stderr, UNKNOWN_OPTION "'%s'.\n", argv[optind - 1]);
            }
            //TODO print responing error
            exit(EXIT_FAILURE);
        }
        switch (opt){
            case 'c':
                if (optarg) {
                    char *endOfPointer;
                    errno = 0;
                    long tmp = strtol(optarg,&endOfPointer,10);
                    if(errno != 0|| *endOfPointer != '\0'|| tmp > INT32_MAX || tmp<0){
                        fprintf(stderr,"%s\n",illegal_argument_cycles);
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
                    if(errno != 0|| *endOfPointer != '\0'|| tmp > UINT32_MAX || tmp<0){
                        fprintf(stderr,"%s\n",illegal_argument_blocksize);
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
                        fprintf(stderr,"%s\n",illegal_argument_blocksize);
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
                        fprintf(stderr,"%s\n",illegal_argument_blocksize);
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
                        fprintf(stderr,"%s\n",illegal_argument_blocksize);
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
                        fprintf(stderr,"%s\n",illegal_argument_blocksize);
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
            case 'h':
                fprintf(stderr,"%s\n",HELP_MSG);
                exit(EXIT_SUCCESS);
            default:
                fprintf(stderr,"%s\n",usage_msg);
                exit(EXIT_FAILURE);
        }
    }


    if(optind <argc) {
        inputfile = argv[optind];
        printf("This is the file %s\n",inputfile);
        //I don't guarantee that it is a file
    } else {
        fprintf(stderr,"%s\n",no_file_input);
        exit(EXIT_FAILURE);
    }
    if(optind <argc - 1) {
        fprintf(stderr,"%s\n",usage_msg_too_many_arguments);
        exit(EXIT_FAILURE);
    }

    fprintf(stderr,"cycles: %i, tlbSize: %u, tlbLatency: %u, blocksize: %u, v2b: %u, memoryLatency: %u, tracefileLocation: %s, Inputfilename: %s",cycles,tlbSize,tlbLatency,blocksize,v2bBlockOffset,memoryLatency,tracefile,inputfile);

    fprintf(stderr,"%s\n",successful);
    return 0;
}

//TODO errors are currently wrong given so wirte errors for every one and set them correctly then lets go....
/* TODO
 *./r5mm --blocksize="3" --blocksize=5 -help name; führt dazu, dass help ausgerufen wird, da -h option verohanden ist und sein Argument elp wäre, ok so eigentlich ja.
 *Theoretisch ist iom optarg :h unnötig, da nach -h, --help gesucht wird davor
 * */