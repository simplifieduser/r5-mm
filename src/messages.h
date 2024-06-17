//
// Created by Lukas Wolf on 11.06.24.
//

#ifndef IO_MESSAGES_H
#define IO_MESSAGES_H

#endif //IO_MESSAGES_H
#include <string.h>
#include "stdint.h"
#include <stdio.h>
//Here are messages to be returned
#define INT_MAX "2147483647"
#define UINT_MAX "4294967295"


//error msg input
#define ILLEGAL_ARGUMENT_PASSING "ERROR: Illegal Argument passing: "

#define NO_FILE_INPUT ILLEGAL_ARGUMENT_PASSING "You are required to forward a file as the positional argument"
#define ILLEGAL_ARGUMENT_CYCLES ILLEGAL_ARGUMENT_PASSING "'cylces (-c/--cycles)' has to be an Integer between [0; " INT_MAX "] "
#define ILLEGAL_ARGUMENT_BLOCKSIZE ILLEGAL_ARGUMENT_PASSING  "'blocksize (-b/--blocksize)' has to be an Integer between [1; " UINT_MAX "]"
#define ILLEGAL_ARGUMENT_V2B_BLOCK_OFFSET ILLEGAL_ARGUMENT_PASSING  "'v2b-block-offset (-o/--v2b-block-offset)' has to be an Integer between [0; " UINT_MAX "]"
#define ILLEGAL_ARGUMENT_TLB_SIZE ILLEGAL_ARGUMENT_PASSING  "'tlb-size(-s/--tlb-size)' has to be an Integer between [0; " UINT_MAX "]"
#define ILLEGAL_ARGUMENT_TLB_LATENCY ILLEGAL_ARGUMENT_PASSING  "'tlb-size(-t/--tlb-latency)' has to be an Integer between [0; " UINT_MAX "]"
#define ILLEGAL_ARGUMENT_MEMORY_LATENCY ILLEGAL_ARGUMENT_PASSING  "'memory-latency(-m/--memory-latency)' has to be an Integer between [0; " UINT_MAX "]"
#define USAGE_MSG  ILLEGAL_ARGUMENT_PASSING "You passed illegal/unknown arguments: " //it may be that this is unnecessary
#define USAGE_TOO_MANY_ARGUMENTS ILLEGAL_ARGUMENT_PASSING  "You passed to many arguments for help please use -h"
#define UNKNOWN_OPTION "ERROR: Unknown option, please remove: "



#define successful  "Program succeeded successfully"
#define HELP_MSG \
"Usage: r5mm\n"  \
"\n"             \
"Description: HERE YOU CAN PUT YOUR ADVERTISEMENT\n" \
"Options:                     default values:               allowed arguments:      Description:\n"     \
"   -c, --cycles                                            [0; 2147483647] int     Set the amount of cycles the program should run\n" \
"   -b, --blocksize                                         [1; 4294967295] uint    Set the size of a block in byte\n"                         \
"   -o, --v2b-block-offset                                  [0; 4294967295] uint    Set the size of the v2b-block-offset, offset between virtual and physical address\n" \
"   -s, --tlb-size                                          [0; 4294967295] uint    Set the size of the tlb, i.e. how many entries can be stored simultaneously\n" \
"   -t, --tlb-latency                                       [0; 4294967295] uint    Set the latency of the tlb, i.e. how many cycles an access needs\n"                    \
"   -m, --memory-latency                                    [0; 4294967295] uint    Set the latency of the memory, i.e. how many cycles an access of the memory needs\n"   \
"   -f, --tf=                 null                          <name-of-file>          If a Tracefile should be created, provide a name\n"                 \
"   <filename>                !must be set manually!        <name-of-file>          Sets the path of the input file. It must be set as a positional argument\n"\
"   -h, --help                --------------------------------------------------    Prints this page\n"
