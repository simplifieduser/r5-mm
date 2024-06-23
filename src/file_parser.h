#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include <stdint.h>

typedef struct Request
{
  uint32_t addr;
  uint32_t data;
  int we;
} Request;

int getLineCount(const char *path);
int parseFile(const char *path, int maxRequestCount, Request requests[]);

#endif