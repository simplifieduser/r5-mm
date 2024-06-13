#ifndef MODULES_HPP
#define MODULES_HPP

#include <systemc>
using namespace sc_core;

SC_MODULE(GET_ADDRESS)
{
    sc_in<uint32_t> virtual_address;
    sc_in<unsigned> tlbSize;
    sc_in<unsigned> tlbsLatency;
    sc_in<unsigned> blocksize;
};

#endif