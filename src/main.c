#include <stdio.h>
#include <stdlib.h>
#include "file_parser.h"

int main(int argc, char const *argv[]) {

    int lineCount = getLineCount("test.csv");
    if (lineCount < 0) { return 1; }

    Request requests[lineCount];
    int requestCount = parseFile("test.csv", lineCount, requests);

    if (requestCount < 0) {
        return 1;
    }

    for (int i = 0; i < requestCount; ++i) {
        Request req = requests[i];
        printf("%d: %d %d %d\n", i, req.we, req.addr, req.data);
    }

    return 0;

}