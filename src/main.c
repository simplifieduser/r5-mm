#include <stdio.h>
#include <stdlib.h>
#include "file_parser.h"

int main(int argc, char const *argv[]) {

    int lineCount = getLineCount("test.csv");
    if (lineCount < 0) { return -1; }

    Request requests[lineCount];
    int requestCount = parseFile("test.csv", lineCount, requests);

    printf("\n\nFINAL COUNT: %d\n", requestCount);

    return requestCount;

}