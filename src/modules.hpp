#ifndef MODULES_HPP
#define MODULES_HPP

#include <systemc>
#include <map>
using namespace sc_core;

struct Request
{
    uint32_t addr;
    uint32_t data;
    int we;
};

SC_MODULE(TLB)
{
    // input
    sc_in<bool> clk;
    sc_in<uint32_t> virtual_address;

    // value at index i in the buffer stores a tag that is currently cached in the tlb
    uint32_t *buffer;
    unsigned tlbs_latency;
    unsigned memory_latency;
    unsigned blocksize;
    unsigned v2b_block_offset;
    unsigned tlb_size;

    // true as soon as a valid physical address has been written to physical_address out-signal
    sc_out<bool> finished;
    sc_out<uint32_t> physical_address;
    // counts the cycles needed to retrieve the address
    sc_out<size_t> cycles;
    // true, if address in tlb, otherwise false
    sc_out<bool> hit;

    int latency;

    SC_CTOR(TLB);

    TLB(sc_module_name name, uint32_t * buffer, unsigned tlb_size, unsigned tlbs_latency, unsigned blocksize, unsigned v2b_block_offset, unsigned memory_latency) : sc_module(name), buffer(buffer), tlb_size(tlb_size), tlbs_latency(tlbs_latency), blocksize(blocksize), v2b_block_offset(v2b_block_offset), memory_latency(memory_latency)
    {
        SC_THREAD(count_latency);
        sensitive << clk.pos();
    }

    void count_latency()
    {
        while (true)
        {
            // not actually necessary, but will let the clock run for latency cycles and then set the values for the new request

            set_values();

            int cycle_count = 0;
            while (cycle_count < latency)
            {
                cycle_count++;
                wait();
            }

            cycles->write(cycle_count);
            finished->write(true);
            wait();
        }
    }

    void set_values()
    {
        // show that address has not been translated yet, because new translation process has started
        finished->write(false);

        // calculate tag of virtual address by shifting to the right by page index bits
        // TODO: figure out how to calculate tag (what exactly does blocksize mean)
        uint32_t tag = virtual_address->read() >> (int)log2(blocksize);
        uint32_t current_tag_in_tlb = buffer[tag % tlb_size];

        // check if our tag is currently cached in the tlb
        if (tag == current_tag_in_tlb)
        {
            latency = tlbs_latency;
            hit->write(true);
        }
        else
        {
            latency = tlbs_latency + memory_latency;
            hit->write(false);
        }

        physical_address->write(((tag + v2b_block_offset) << (int)log2(blocksize)) + (virtual_address->read() % (int)log2(blocksize)));
    }
};

SC_MODULE(MAIN_MEMORY)
{
    // input
    sc_in<bool> clk;
    sc_in<bool> we;
    sc_in<uint32_t> data;
    sc_in<uint32_t> address;

    unsigned memory_latency;
    // stores address as key and data as value
    std::map<uint32_t, uint32_t> mem_map;

    sc_out<bool> finished;
    sc_out<uint32_t> out_data;

    SC_CTOR(MAIN_MEMORY);

    MAIN_MEMORY(sc_module_name name, std::map<uint32_t, uint32_t> & map, unsigned memory_latency) : sc_module(name), mem_map(map), memory_latency(memory_latency)
    {
        SC_THREAD(start_process);
        sensitive << clk.pos();
    }

    void start_process()
    {
        while (true)
        {
            update_memory();
            int cycle_counter = 0;
            finished->write(false);

            while (cycle_counter < memory_latency)
            {
                cycle_counter++;
                wait();
            }
            finished->write(true);
            wait();
        }
    }

    void update_memory()
    {
        if (we->read() == true)
        {
            char data_bytes[4];
            data_bytes[3] = data->read() & 0xFF;
            data_bytes[2] = (data->read() >> 8) & 0xFF;
            data_bytes[1] = (data->read() >> 16) & 0xFF;
            data_bytes[0] = (data->read() >> 24) & 0xFF;

            // most significant bit stored at address
            mem_map[address->read()] = data_bytes[0];
            mem_map[address->read() + 1] = data_bytes[1];
            mem_map[address->read() + 2] = data_bytes[2];
            mem_map[address->read() + 3] = data_bytes[3];
        }
        else
        {
            char data_bytes[4];

            data_bytes[0] = mem_map[address->read()];
            data_bytes[1] = mem_map[address->read() + 1];
            data_bytes[2] = mem_map[address->read() + 2];
            data_bytes[3] = mem_map[address->read() + 3];

            out_data->write((data_bytes[0] << 24) + (data_bytes[1] << 16) + (data_bytes[2] << 8) + data_bytes[3]);
        }
    }
};

SC_MODULE(REQUEST_PROCESSOR)
{
    sc_in<bool> clk;
    TLB address_getter;
    MAIN_MEMORY data_manager;
    uint32_t *buffer;
    std::map<uint32_t, uint32_t> mem_map;

    sc_out<size_t> cycles, misses, hits, primitive_gate_count;

    SC_CTOR(REQUEST_PROCESSOR);
    REQUEST_PROCESSOR(sc_module_name name, unsigned tlb_size, unsigned tlbs_latency, unsigned blocksize, unsigned v2b_block_offset, unsigned memory_latency, size_t num_requests, Request *requests) : sc_module(name), buffer(new uint32_t[tlb_size]), address_getter("address_getter", buffer, tlb_size, tlbs_latency, blocksize, v2b_block_offset, memory_latency), data_manager("data_manager", mem_map, memory_latency)
    {
    }
};
#endif