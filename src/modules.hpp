#ifndef MODULES_HPP
#define MODULES_HPP

#include <systemc>
#include <map>
#include <unordered_set>
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
    // stores which values have been previously cached to catch cold misses
    std::unordered_set<uint32_t> previuosly_visited;
    // these values are set once and don't change
    unsigned tlbs_latency,
        memory_latency, blocksize, v2b_block_offset, tlb_size;

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

            wait(start.posedge_event());
            wait();
            wait(SC_ZERO_TIME);
            // not actually necessary, but will let the clock run for latency cycles and then set the values for the new request

            // show that address has not been translated yet, because new translation process has started
            finished->write(false);

            set_values();

            int cycle_count = 0;
            while (cycle_count < latency)
            {
                cycle_count++;
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
        // TODO: figure out how to calculate tag (what exactly does blocksize mean)
        uint32_t tag = virtual_address->read() >> (int)log2(blocksize);
        uint32_t current_tag_in_tlb = buffer[tag % tlb_size];

        std::cout << "tag: " << tag << std::endl;
        std::cout << "currently cached tag: " << current_tag_in_tlb << std::endl;

        // check if our tag is currently cached in the tlb
        if (tag == current_tag_in_tlb && previuosly_visited.count(tag))
        {
            std::cout << "hit" << std::endl;
            previuosly_visited.insert(tag);
            latency = tlbs_latency;
            hit->write(true);
        }
        else
        {
            std::cout << "miss, because: \ntag is wrong: " << (current_tag_in_tlb != tag) << "cold miss: " << (previuosly_visited.count(tag) == 0) << std::endl;

            previuosly_visited.insert(tag);
            buffer[tag % tlb_size] = tag;
            latency = tlbs_latency + memory_latency;
            hit->write(false);
        }

        physical_address->write(((tag + v2b_block_offset) << (int)log2(blocksize)) + (virtual_address->read() % (int)log2(blocksize)));
        std::cout << "calculated physical address: " << (((tag + v2b_block_offset) << (int)log2(blocksize)) + (virtual_address->read() % (int)log2(blocksize))) << std::endl;
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
    std::map<uint32_t, unsigned char> mem_map;

    sc_out<bool> finished;
    sc_out<uint32_t> out_data;

    SC_CTOR(MAIN_MEMORY);

    MAIN_MEMORY(sc_module_name name, unsigned memory_latency) : sc_module(name), memory_latency(memory_latency)
    {
        SC_THREAD(start_process);
        sensitive << clk.pos();
    }

    void start_process()
    {
        wait(clk.posedge_event()); // start on rising edge of the clock (wait for data to be set)
        while (true)
        {

            wait(got_address.posedge_event());
            wait(SC_ZERO_TIME);

            finished->write(false);

            update_memory();

            int cycle_counter = 0;

            while (cycle_counter < memory_latency)
            {
                cycle_counter++;
                wait();
            }
            wait(SC_ZERO_TIME);
            finished->write(true);
        }
    }

    void update_memory()
    {
        if (we->read() == true)
        {
            unsigned char data_bytes[4];
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
            unsigned char data_bytes[4];

            data_bytes[0] = mem_map[address->read()];
            data_bytes[1] = mem_map[address->read() + 1];
            data_bytes[2] = mem_map[address->read() + 2];
            data_bytes[3] = mem_map[address->read() + 3];

            out_data->write((data_bytes[0] << 24) + (data_bytes[1] << 16) + (data_bytes[2] << 8) + data_bytes[3]);

            std::cout << out_data.read() << std::endl;
        }
    }
};

SC_MODULE(REQUEST_PROCESSOR)
{
    sc_in<bool> clk;

    // fixed values for this tlb
    unsigned tlb_size, tlbs_latency, blocksize, v2b_block_offset, memory_latency;
    size_t num_requests;
    Request *requests;

    ADDRESS_GETTER address_getter;
    MAIN_MEMORY data_manager;
    std::vector<uint32_t> buffer; // stores which virtual addresses are currently in tlb

    sc_signal<uint32_t> current_virt_address, current_phys_address, data, out_data;
    sc_signal<size_t> addr_cycles;
    sc_signal<bool> get_address_finished, manage_data_finished, we, start_request, hit, notify_memory;

    sc_out<size_t> cycles, misses, hits, primitive_gate_count;

    SC_CTOR(REQUEST_PROCESSOR);
    REQUEST_PROCESSOR(sc_module_name name, unsigned tlb_size, unsigned tlbs_latency, unsigned blocksize, unsigned v2b_block_offset, unsigned memory_latency, size_t num_requests, Request *requests, std::vector<uint32_t> buffer_param) : sc_module(name), tlb_size(tlb_size), tlbs_latency(tlbs_latency), blocksize(blocksize), v2b_block_offset(v2b_block_offset), memory_latency(memory_latency), num_requests(num_requests), requests(requests), buffer(buffer_param), address_getter("address_getter", buffer_param, tlb_size, tlbs_latency, blocksize, v2b_block_offset, memory_latency), data_manager("data_manager", memory_latency)
    {
        // TODO: bind hit and cycles for address getter
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
        for (int i = 0; i < num_requests; i++)
        {
            wait(SC_ZERO_TIME);
            std::cout << "\n\n"
                      << i << ") current address: " << (requests[i].addr)
                      << std::endl;
            // set signals according to values in current request
            data.write(requests[i].data);
            we.write(requests[i].we);
            current_virt_address.write(requests[i].addr);
            if (i == 4)
                std::cout << requests[i].addr << std::endl;

            start_request.write(true);

            // first, wait for correct physical address to be fetched
            wait(get_address_finished.posedge_event());
            wait(SC_ZERO_TIME);

            // now update with returned values
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
        }
        sc_stop();
    }
};
#endif