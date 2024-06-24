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

SC_MODULE(ADDRESS_GETTER)
{
    // input
    sc_in<bool> clk;
    sc_in<uint32_t> virtual_address;
    sc_in<bool> start;

    // value at index i in the buffer stores a tag that is currently cached in the tlb
    std::vector<uint32_t> buffer;
    // these values are set once and don't change
    unsigned tlbs_latency, memory_latency, blocksize, v2b_block_offset, tlb_size;

    // true as soon as a valid physical address has been written to physical_address out-signal
    sc_out<bool> finished;
    sc_out<uint32_t> physical_address;
    // counts the cycles needed to retrieve the address
    sc_out<size_t> cycles;
    // true, if address in tlb, otherwise false
    sc_out<bool> hit;

    int latency;

    SC_CTOR(ADDRESS_GETTER);

    ADDRESS_GETTER(sc_module_name name, std::vector<uint32_t> buffer, unsigned tlb_size, unsigned tlbs_latency, unsigned blocksize, unsigned v2b_block_offset, unsigned memory_latency) : sc_module(name), buffer(buffer), tlb_size(tlb_size), tlbs_latency(tlbs_latency), blocksize(blocksize), v2b_block_offset(v2b_block_offset), memory_latency(memory_latency)
    {
        SC_THREAD(count_latency);
        sensitive << clk.pos();
    }

    void count_latency()
    {
        while (true)
        {
            // wait for start_signal
            while (start->read() == false)
            {
                wait();
            }
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
    sc_in<bool> got_address; // true, as soon as correct address has been calculated, can then start writing/reading data

    unsigned memory_latency;
    // stores address as key and data as value
    std::map<uint32_t, char> mem_map;

    sc_out<bool> finished;
    sc_out<uint32_t> out_data;

    SC_CTOR(MAIN_MEMORY);

    MAIN_MEMORY(sc_module_name name, std::map<uint32_t, char> & map, unsigned memory_latency) : sc_module(name), mem_map(map), memory_latency(memory_latency)
    {
        SC_THREAD(start_process);
        sensitive << clk.pos();
    }

    void start_process()
    {
        while (true)
        {
            while (got_address->read() == false)
            {
                wait();
            }
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

    // fixed values for this tlb
    unsigned tlb_size, tlbs_latency, blocksize, v2b_block_offset, memory_latency;
    size_t num_requests;
    std::vector<Request> requests;

    ADDRESS_GETTER address_getter;
    MAIN_MEMORY data_manager;
    std::vector<uint32_t> buffer;     // stores which virtual addresses are currently in tlb
    std::map<uint32_t, char> mem_map; // represents the main memory

    sc_signal<uint32_t> current_virt_address, current_phys_address, data, addr_cycles;
    sc_signal<bool> get_address_finished, manage_data_finished, we, start_request, hit;

    sc_out<size_t> cycles, misses, hits, primitive_gate_count;
    sc_out<bool> finished;

    SC_CTOR(REQUEST_PROCESSOR);
    REQUEST_PROCESSOR(sc_module_name name, unsigned tlb_size, unsigned tlbs_latency, unsigned blocksize, unsigned v2b_block_offset, unsigned memory_latency, size_t num_requests, std::vector<Request> requests, std::vector<uint32_t> buffer_param) : sc_module(name), tlb_size(tlb_size), tlbs_latency(tlbs_latency), blocksize(blocksize), v2b_block_offset(v2b_block_offset), memory_latency(memory_latency), num_requests(num_requests), requests(requests), buffer(buffer_param), address_getter("address_getter", buffer_param, tlb_size, tlbs_latency, blocksize, v2b_block_offset, memory_latency), data_manager("data_manager", mem_map, memory_latency)
    {
        // TODO: bind hit and cycles for address getter
        address_getter.clk.bind(clk);
        address_getter.virtual_address.bind(current_virt_address);
        address_getter.physical_address.bind(current_phys_address);
        address_getter.finished.bind(get_address_finished);
        address_getter.start.bind(start_request);
        address_getter.cycles(cycles);
        address_getter.hit(hit);

        data_manager.clk.bind(clk);
        data_manager.address.bind(address_getter.physical_address);
        data_manager.data.bind(data);
        data_manager.we.bind(we);
        data_manager.got_address.bind(address_getter.finished);

        finished->write(false);

        SC_THREAD(run_program);
    }

    void run_program()
    {
        for (int i = 0; i < num_requests; i++)
        {
            // set signals according to values in current request
            data.write(requests[i].data);
            we.write(requests[i].we);
            current_virt_address.write(requests[i].addr);
            start_request.write(true);

            // first, wait for correct physical address to be fetched
            while (get_address_finished == false)
            {
                wait();
            }

            // now update with returned values
            cycles->write(cycles->read() + address_getter.cycles->read());
            hits->write(hits->read() + address_getter.hit->read());
            misses->write(misses->read() + !address_getter.hit->read());
            start_request.write(false);

            // wait, while data is being read/written
            while (manage_data_finished == false)
            {
                wait();
            }
            // update cycles again
            cycles->write(cycles->read() + memory_latency);
        }

        finished->write(true);
    }
};
#endif