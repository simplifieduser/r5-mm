#include <systemc>

#include "modules.hpp"

int run_simulation(int seconds)
{
    return 0;
}

// Note that we need this default sc_main implementation.
int sc_main(int argc, char *argv[])
{
    std::cout << "Should not be called directly" << std::endl;
    return -1;
}