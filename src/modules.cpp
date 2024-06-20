#include <systemc>

#include "modules.hpp"

struct Result
{
    size_t cycles;
    size_t misses;
    size_t hits;
    size_t primitive_gate_count;
};

struct Request
{
    uint32_t addr;
    uint32_t data;
    int we;
};

Result run_simulation(int cycles, unsigned tlbSize, unsigned tlbsLatency, unsigned blocksize, unsigned v2bBlockOffset, unsigned memoryLatency, size_t numRequests, Request *requests, const char *tracefile)
{
    Result result;

    return result;
}

// Note that we need this default sc_main implementation.
int sc_main(int argc, char *argv[])
{
    std::cout << "Should not be called directly" << std::endl;
    return -1;
}