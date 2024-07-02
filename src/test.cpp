#include <systemc>
#include "modules.cpp"

int main(int argc, char *argv[])
{
    Request r1;
    r1.addr = 1234;
    r1.data = 13;
    r1.we = 1;

    Request r2;
    r2.addr = 1234;
    r2.data = 0;
    r2.we = 0;

    Request r3;
    r3.addr = 5233;
    r3.data = 12;
    r3.we = 1;

    Request r4;
    r4.addr = 5233;
    r4.data = 0;
    r4.we = 0;

    Request r5;
    r5.addr = 5234;
    r5.data = 0;
    r5.we = 0;

    Request requests[5];
    requests[0] = r1;
    requests[1] = r2;
    requests[2] = r3;
    requests[3] = r4;
    requests[4] = r5;

    Result res = run_simulation(10000, 64, 1, 4, 4, 100, 5, requests, "trace");

    std::cout << res.cycles << "\n"
              << res.hits << "\n"
              << res.misses << std::endl;

    return 0;
}