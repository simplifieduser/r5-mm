#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include <stdint.h>
#include "shared.h"

int parseFile(const char *path, Request **requests);

#endif
