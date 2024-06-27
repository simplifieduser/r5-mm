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

    Request requests[1];
    requests[0] = r1;
    // requests[1] = r2;

    Result res = run_simulation(100, 10, 2, 8, 3, 10, 1, requests, "trace");

    std::cout << res.cycles << "\n"
              << res.hits << "\n"
              << res.misses << std::endl;

    return 0;
}