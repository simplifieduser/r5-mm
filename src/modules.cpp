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
    // TODO: primitive_gate_count

    Result result;                         // to be returned
    std::vector<uint32_t> buffer(tlbSize); // stores which virtual addresses are in the tlb
    sc_clock clk;

    sc_signal<size_t> misses, hits, primitive_gate_count;
    sc_signal<int> cycles_count;

    REQUEST_PROCESSOR request_processor("request_processor", tlbSize, tlbsLatency, blocksize, v2bBlockOffset, memoryLatency, numRequests, requests, buffer);
    request_processor.clk.bind(clk);
    request_processor.cycles.bind(cycles_count);
    request_processor.misses.bind(misses);
    request_processor.hits.bind(hits);
    request_processor.primitive_gate_count.bind(primitive_gate_count);

    // create new tracefile and set the signals to be traced (clock and finished are unimportant)
    sc_trace_file *file = NULL;
    if (tracefile != NULL)
    {
        sc_trace_file *file = sc_create_vcd_trace_file(tracefile);
        sc_trace(file, cycles_count, "cycles");
        sc_trace(file, misses, "misses");
        sc_trace(file, hits, "hits");
        sc_trace(file, primitive_gate_count, "gate_count");
    }

    sc_start();

    // set result values to output from request_processor
    result.hits = request_processor.hits.read();
    result.misses = request_processor.misses.read();
    result.primitive_gate_count = request_processor.primitive_gate_count.read();

    if (cycles_count.read() > cycles)
    {
        result.cycles = INT_MAX;
    }
    else
    {
        result.cycles = cycles_count.read();
    }
    if (tracefile != NULL)
        sc_close_vcd_trace_file(file);

    return result;
}

// Note that we need this default sc_main implementation.
int sc_main(int argc, char *argv[])
{
    std::cout << "Should not be called directly" << std::endl;
    return -1;
}