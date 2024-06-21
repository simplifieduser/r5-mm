#include <systemc>

#include "modules.hpp"

struct Result
{
    size_t cycles;
    size_t misses;
    size_t hits;
    size_t primitive_gate_count;
};

Result run_simulation(int cycles, unsigned tlbSize, unsigned tlbsLatency, unsigned blocksize, unsigned v2bBlockOffset, unsigned memoryLatency, size_t numRequests, Request *requests, const char *tracefile)
{
    Result result;            // to be returned
    uint32_t buffer[tlbSize]; // stores which virtual addresses are in the tlb
    sc_clock clk;

    sc_signal<size_t> cycles_count, misses, hits, primitive_gate_count;
    sc_signal<bool> finished; // marks, when all the requests have been processed

    REQUEST_PROCESSOR request_processor("request_processor", tlbSize, tlbsLatency, blocksize, v2bBlockOffset, memoryLatency, numRequests, requests, buffer);

    request_processor.clk.bind(clk);
    request_processor.cycles.bind(cycles_count);
    request_processor.misses.bind(misses);
    request_processor.hits.bind(hits);
    request_processor.primitive_gate_count.bind(primitive_gate_count);
    request_processor.finished.bind(finished);

    sc_start();
    while (finished.read() == false)
    {
        // wait, until all requests have been processed
    }

    // set result values to output from request_processor
    result.hits = hits.read();
    result.misses = misses.read();
    result.primitive_gate_count = primitive_gate_count.read();

    if (cycles_count.read() > cycles)
    {
        result.cycles = INT_MAX;
    }
    else
    {
        result.cycles = cycles_count.read();
    }

    return result;
}

// Note that we need this default sc_main implementation.
int sc_main(int argc, char *argv[])
{
    std::cout << "Should not be called directly" << std::endl;
    return -1;
}