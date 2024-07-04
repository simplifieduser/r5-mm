#ifndef MEMORY_MODULE_HPP
#define MEMORY_MODULE_HPP

#include <systemc>
#include <map>
using namespace sc_core;

SC_MODULE(MAIN_MEMORY)
{
    // input
    sc_in<bool> clk;
    sc_in<bool> we;
    sc_in<uint32_t> data;
    sc_in<uint32_t> address;
    sc_in<bool> got_address; // true, as soon as correct address has been retrieved, can then start writing/reading data (sync with address_module)

    unsigned memory_latency;

    // simulate main memory: stores address as key and data as value
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
        wait(clk.posedge_event());
        while (true)
        {

            wait(got_address.posedge_event());
            wait(SC_ZERO_TIME);

            finished->write(false);

            // read from / write to memory
            update_memory();

            // wait for cycles to pass to simulate required time

            unsigned int cycle_counter = 0;

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
            if (mem_map.count(address->read()))
            {
                unsigned char data_bytes[4];

                data_bytes[0] = mem_map[address->read()];
                data_bytes[1] = mem_map[address->read() + 1];
                data_bytes[2] = mem_map[address->read() + 2];
                data_bytes[3] = mem_map[address->read() + 3];

                out_data->write((data_bytes[0] << 24) + (data_bytes[1] << 16) + (data_bytes[2] << 8) + data_bytes[3]);
            }
            else
            {
                out_data->write(0);
            }
        }
    }
};
#endif