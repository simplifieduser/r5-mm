#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include <stdint.h>

typedef struct Request
{
  uint32_t addr;
  uint32_t data;
  int we;
} Request;

int parseFile(const char *path, Request **requests);

#endif
