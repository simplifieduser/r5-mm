#include "modules.hpp"

Result run_simulation(int cycles, unsigned tlbSize, unsigned tlbsLatency, unsigned blocksize, unsigned v2bBlockOffset, unsigned memoryLatency, size_t numRequests, Request *requests, const char *tracefile)
{
    Result result;                         // Speichert Rückgabewerte
    std::vector<uint32_t> buffer(tlbSize); // Enthält alle aktuell im TLB gespeicherten Tags
    sc_clock clk;

    // Übergebenen Parameter
    sc_signal<size_t> misses, hits, primitive_gate_count;
    sc_signal<int> cycles_count;

    // Bearbeitet die Requests und gibt die gesuchten Werte zurück
    REQUEST_PROCESSOR request_processor("request_processor", tlbSize, tlbsLatency, blocksize, v2bBlockOffset, memoryLatency, numRequests, requests, buffer);
    request_processor.primitive_gate_count.bind(primitive_gate_count);
    request_processor.clk.bind(clk);
    request_processor.cycles.bind(cycles_count);
    request_processor.misses.bind(misses);
    request_processor.hits.bind(hits);

    // Tracefile erstellen und Signale einfügen, falls diese erstellt werden soll (tracefile != NULL)
    sc_trace_file *file = NULL;
    if (tracefile != NULL)
    {
        sc_trace_file *file = sc_create_vcd_trace_file(tracefile);
        sc_trace(file, clk, "clock");
        sc_trace(file, cycles_count, "cycles");
        sc_trace(file, misses, "misses");
        sc_trace(file, hits, "hits");
        sc_trace(file, primitive_gate_count, "gate_count");
    }

    // Simulation starten
    sc_start();

    // Rückgabewerte auf Output von request_processor setzen
    result.hits = hits.read();
    result.misses = misses.read();
    result.primitive_gate_count = primitive_gate_count.read();

    // Überprüfen, ob die Anzahl von Zyklen unter dem übergebenen Limit liegt
    if (cycles_count.read() > cycles)
    {
        // zu viele Zyklen gebraucht
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

// Wir brauchen diese default sc_main Implementation.
int sc_main(int argc, char *argv[])
{
    std::cout << "Should not be called directly" << std::endl;
    return -1;
}
