#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include <stdint.h>

struct Request
{
  uint32_t addr;
  uint32_t data;
  int we;
};

int parseFile(const char* path);

#endif