#ifndef MODULES_HPP
#define MODULES_HPP

#include <systemc>
#include "shared.h"
#include "address_module.hpp"
#include "memory_module.hpp"
using namespace sc_core;

SC_MODULE(REQUEST_PROCESSOR)
{
    sc_in<bool> clk;

    // tlb properties
    unsigned tlb_size, tlbs_latency, blocksize, v2b_block_offset, memory_latency;
    size_t num_requests;
    Request *requests;

    ADDRESS_GETTER address_getter;
    MAIN_MEMORY data_manager;
    std::vector<uint32_t> buffer; // stores which virtual addresses are currently in tlb

    // signals, to connect modules with each other
    sc_signal<uint32_t> current_virt_address, current_phys_address, data, out_data;
    sc_signal<int> addr_cycles;
    sc_signal<bool> get_address_finished, manage_data_finished, we, start_request, hit, notify_memory;

    sc_out<size_t> misses, primitive_gate_count, hits;
    sc_out<int> cycles;

    SC_CTOR(REQUEST_PROCESSOR);
    REQUEST_PROCESSOR(sc_module_name name, unsigned tlb_size, unsigned tlbs_latency, unsigned blocksize, unsigned v2b_block_offset, unsigned memory_latency, size_t num_requests, Request *requests, std::vector<uint32_t> buffer_param) : sc_module(name), tlb_size(tlb_size), tlbs_latency(tlbs_latency), blocksize(blocksize), v2b_block_offset(v2b_block_offset), memory_latency(memory_latency), num_requests(num_requests), requests(requests), address_getter("address_getter", buffer_param, tlb_size, tlbs_latency, blocksize, v2b_block_offset, memory_latency), data_manager("data_manager", memory_latency), buffer(buffer_param)
    {
        address_getter.clk.bind(clk);
        address_getter.virtual_address.bind(current_virt_address);
        address_getter.physical_address.bind(current_phys_address);
        address_getter.finished.bind(get_address_finished);
        address_getter.start.bind(start_request);
        address_getter.cycles.bind(addr_cycles);
        address_getter.hit(hit);

        data_manager.clk.bind(clk);
        data_manager.address.bind(current_phys_address);
        data_manager.data.bind(data);
        data_manager.we.bind(we);
        data_manager.got_address.bind(notify_memory);
        data_manager.out_data.bind(out_data);
        data_manager.finished.bind(manage_data_finished);

        start_request.write(false);
        notify_memory.write(false);

        SC_THREAD(run_program);
        sensitive << clk.pos();
    }

    void run_program()
    {
        // set gates needed for size of the tlb (tag size + address size (32 Bits): gates per row)
        primitive_gate_count->write((64 - (log2(blocksize)) + 1) * 4 * tlb_size);

        for (size_t i = 0; i < num_requests; i++)
        {
            wait(SC_ZERO_TIME);
            // set signals according to values in current request
            data.write(requests[i].data);
            we.write(requests[i].we);
            current_virt_address.write(requests[i].addr);

            start_request.write(true);

            // first, wait for correct physical address to be fetched
            wait(get_address_finished.posedge_event());
            wait(SC_ZERO_TIME);

            // now update output with returned values
            cycles->write(cycles->read() + address_getter.cycles->read());
            hits->write(hits->read() + address_getter.hit->read());
            misses->write(misses->read() + !address_getter.hit->read());
            start_request.write(false);
            notify_memory.write(true);

            // wait, while data is being read/written
            wait(manage_data_finished.posedge_event());
            wait(SC_ZERO_TIME);
            notify_memory.write(false);

            // update cycles again
            cycles->write(cycles->read() + memory_latency);
            wait();

            // update number of used gates
            // 182 = 150 for addition of 2 32-bit numbers + 32 xor gates for two's complement
            primitive_gate_count->write(primitive_gate_count->read() + 182);
        }
        sc_stop();
    }
};
#endif
