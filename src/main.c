#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdint.h>
#include <errno.h>
#include "messages.h"

void print(const char* msg){
    printf("%s\n", msg);
}
int main(int argc, char*argv[]) {
    int cycles;
    unsigned int tlbSize;
    unsigned int tlbLatency;
    unsigned int blocksize;
    unsigned int v2bBlockOffset;
    unsigned int memoryLatency;
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
            {"tf=",required_argument,0,'f'},
            {"help", no_argument,0,'h'},
            {0,0,0,0}
    };

    while ((opt = getopt_long(argc, argv, "c::b::o::s::t::m::f:h", long_options, &optional_index))!= -1){
        switch (opt){
            case 'c':
                if(optarg) {
                    char *endOfPointer;
                    errno = 0;
                    long tmp = strtol(optarg,&endOfPointer,10);
                    if(errno != 0|| *endOfPointer != '\0'|| tmp > INT32_MAX || tmp<0){
                        print(passing_error_cycles);
                        exit(EXIT_FAILURE);
                    }
                    cycles = (int) tmp;
                    printf("this was passed for cycle: %i\n",cycles);
                }
                break;
                //TODO apparently when there is a blank between -c 12 file, then 12 is not recognized
            case 'b':{
                if(optarg) {
                    char *endOfPointer;
                    errno = 0;
                    long tmp = strtol(optarg,&endOfPointer,10);
                    if(errno != 0|| *endOfPointer != '\0'|| tmp > UINT32_MAX || tmp<=0){
                        print(passing_error_blocksize);
                        exit(EXIT_FAILURE);
                    }
                    blocksize = (unsigned) tmp;
                    printf("this was passed for cycle: %i\n",blocksize);
                }
            }
            case 'o':{

            }
            case 's':{

            }
            case 't':{

            }
            case 'm':{

            }
            case 'f':{

            }
            case 'h':
                print(help_msg);
                exit(EXIT_SUCCESS);
            default:
                print(usage_msg);
                exit(EXIT_FAILURE);
        }
    }


    if(optind <argc) {
        inputfile = argv[optind];
        //I don't guarantee that it is a file
    } else {
        print(no_file_input);
        exit(EXIT_FAILURE);
    }
    if(optind <argc - 1) {
        print(usage_msg_too_many_arguments);
        exit(EXIT_FAILURE);
    }

    print(successful);
    return 0;
}
