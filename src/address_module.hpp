#ifndef ADDRESS_MODULE_HPP
#define ADDRESS_MODULE_HPP

#include <systemc>
#include <unordered_set>
using namespace sc_core;

SC_MODULE(ADDRESS_GETTER)
{
    // input
    sc_in<bool> clk;
    sc_in<uint32_t> virtual_address;
    sc_in<bool> start;

    // value at index i in the buffer stores a tag that is currently cached in the tlb
    std::vector<uint32_t> buffer;

    // stores which values have been previously cached to catch cold misses
    std::unordered_set<uint32_t> previuosly_visited;

    // properties of the tlb
    unsigned tlbs_latency,
        memory_latency, blocksize, v2b_block_offset, tlb_size;

    // true as soon as a valid physical address has been written to physical_address out-signal
    sc_out<bool> finished;
    sc_out<uint32_t> physical_address;

    // counts the cycles needed to retrieve the address
    sc_out<int> cycles;

    // true, if address was in the tlb, otherwise false
    sc_out<bool> hit;

    // set to tlb_latency or memory_latency, depending on whether the address was in the tlb
    int latency;

    SC_CTOR(ADDRESS_GETTER);

    ADDRESS_GETTER(sc_module_name name, std::vector<uint32_t> buffer, unsigned tlb_size, unsigned tlbs_latency, unsigned blocksize, unsigned v2b_block_offset, unsigned memory_latency) : sc_module(name), buffer(buffer), tlbs_latency(tlbs_latency), memory_latency(memory_latency), blocksize(blocksize), v2b_block_offset(v2b_block_offset), tlb_size(tlb_size)
    {
        SC_THREAD(count_latency);
        sensitive << clk.pos();
    }

    void count_latency()
    {
        while (true)
        {

            wait(start.posedge_event());
            wait(SC_ZERO_TIME);

            // show that address has not been translated yet, because new translation process has started
            finished->write(false);

            // calculates physical address and determines latency
            set_values();

            // let required cycles pass to simulate the required time for the tlb
            int cycle_count = 0;
            while (cycle_count < latency)
            {
                cycle_count++;
                cycles->write(cycle_count);
                wait();
            }
            wait(SC_ZERO_TIME);
            cycles->write(cycle_count);
            finished->write(true);
        }
    }

    void set_values()
    {
        // calculate tag of virtual address by shifting to the right by page index bits
        uint32_t tag = virtual_address->read() >> (int)log2(blocksize);
        uint32_t current_tag_in_tlb = buffer[tag % tlb_size];

        // check if tag is currently cached in the tlb
        if (tag == current_tag_in_tlb && previuosly_visited.count(tag))
        {
            previuosly_visited.insert(tag);
            latency = tlbs_latency;
            hit->write(true);
        }
        else
        {
            previuosly_visited.insert(tag);
            buffer[tag % tlb_size] = tag;
            latency = tlbs_latency + memory_latency;
            hit->write(false);
        }

        // construct the physical address
        physical_address->write(((tag + v2b_block_offset) << (int)log2(blocksize)) + (virtual_address->read() % (int)log2(blocksize)));
    }
};
#endif