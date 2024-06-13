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

#define ILLEGAL_ARGUMENT_PASSING "ERROR: Illegal Argument passing: "
#define UNKNOWN_OPTION "ERROR: Unknown option, please remove: "
#define INPUT_FILE_PARSING_ERROR "Error: You must provide an inputfile, make sure to include -"
#define usage_msg  "You passed illegal arguments or "
#define usage_msg_too_many_arguments  "You passed to many arguments"
#define no_file_input  "You are requiered to forward a file as the positional argument"
#define illegal_argument_cycles ILLEGAL_ARGUMENT_PASSING "'cylces (-c/--cycles)' muss ein Integer im Interval [0; " INT_MAX "] sein"
#define illegal_argument_blocksize ILLEGAL_ARGUMENT_PASSING  "'blocksize (-b/--blocksize)' muss ein Integer im Interval [1; " UINT_MAX "] sein"
#define successful  "Program succeeded successfully"
#define HELP_MSG \
"Usage: r5mm\n"  \
"\n"             \
"Description: HERE YOU CAN PUT YOUR ADVERTISEMENT\n" \
"Options:                     default value:                allowed argument:           Description:\n"     \
"   -c, --cycles                                            [0; 2147483647] int     Set the amount of cycles the program should run\n" \
"   -b, --blocksize                                         [1; 4294967295] uint    Set the size of a block in byte\n"                         \
"   -o, --v2b-block-offset                                  [0; 4294967295] uint    Set the size of the v2b-block-offset, offset between virtual and physical address\n" \
"   <filename>                !must be set manually!        <path-to-file>          Sets the input file. It must be set as a positional argument\n"\
"   -h, --help                --------------------------------------------------    Prints this page\n"
