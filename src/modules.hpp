#ifndef MODULES_HPP
#define MODULES_HPP

#include <systemc>
using namespace sc_core;

SC_MODULE(GET_ADDRESS)
{
    sc_in<bool> clk;

    sc_in<uint32_t> virtual_address;
    // value at index i in the vector stores a tag that is currently cached in the tlb
    sc_in<sc_vector<uint32_t>> tlb;
    sc_in<unsigned> tlbs_latency;
    sc_in<unsigned> memory_latency;
    sc_in<unsigned> blocksize;
    sc_in<unsigned> v2b_block_offset;
    sc_in<unsigned> tlb_size;

    // true as soon as a valid physical address has been written to physical_address out-signal
    sc_out<bool> finished;
    sc_out<uint32_t> physical_address;
    // counts the cycles needed to retrieve the address
    sc_out<size_t> cycles;
    // true, if address in tlb, otherwise false
    sc_out<bool> hit;

    uint32_t tag;
    uint32_t current_tag_in_tlb;
    int latency;

    SC_CTOR(GET_ADDRESS)
    {
        SC_THREAD(count_latency);
        sensitive << clk.pos();
    }

    void count_latency()
    {
        while (true)
        {
            set_values();

            int cycle_count = 0;
            while (cycle_count < latency)
            {
                cycle_count++;
                cycles->write(cycles->read() + 1);
                wait();
            }

            finished->write(true);
            wait();
        }
    }

    void set_values()
    {
        finished->write(false);
        tag = virtual_address->read() >> (int)log2(blocksize);
        current_tag_in_tlb = tlb->read()[(tag % tlb_size->read())];

        if (tag == current_tag_in_tlb)
        {
            latency = tlbs_latency->read();
            hit->write(true);
        }
        else
        {
            latency = tlbs_latency->read() + memory_latency->read();
            hit->write(false);
        }

        physical_address->write(((tag + v2b_block_offset->read()) << (int)log2(blocksize)) + virtual_address->read() % (int)log2(blocksize));
    }
};

#endif