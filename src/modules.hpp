#ifndef MODULES_HPP
#define MODULES_HPP

#include <systemc>
using namespace sc_core;

SC_MODULE(TLB)
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

    SC_CTOR(TLB)
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
        tag = virtual_address->read() >> (int)log2(blocksize);
        current_tag_in_tlb = tlb->read().at((tag % tlb_size->read()));

        // check if our tag is currently cached in the tlb
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

SC_MODULE(MAIN_MEMORY)
{
    sc_in<bool> clk;
    sc_in<bool> we;
    sc_in<uint32_t> data;
    sc_in<uint32_t> address;
    sc_in<unsigned> memory_latency;
    // missing: memory map bzw. access to main memory

    sc_out<bool> finished;
    sc_out<uint32_t> data;
};
#endif