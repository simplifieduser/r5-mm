#include <stdio.h>
#include <stdlib.h>
#include "file_parser.h"

int main(int argc, char const *argv[])
{

  unsigned count = parseFile("test.csv");

  return count;

}