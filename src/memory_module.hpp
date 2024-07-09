#ifndef MEMORY_MODULE_HPP
#define MEMORY_MODULE_HPP

#include <systemc>
#include <map>
using namespace sc_core;

SC_MODULE(MAIN_MEMORY)
{
    // Input
    sc_in<bool> clk;
    sc_in<bool> we;          // Write enable -> 1 wenn Schreibzugriff, 0 wenn Lesezugriff
    sc_in<uint32_t> data;    // Daten die evtl. gespeichert werden sollen
    sc_in<uint32_t> address; // physikalische Adresse
    sc_in<bool> got_address; // True, sobald richtige physikalische Adresse am address-Signal liegt, erst dann können Daten gelesen / geschrieben werden

    unsigned memory_latency; // benötigte Zyklen für die Speicheropertaion

    // Simuliert den Hauptspeicher: speichert Adresse als Key und Daten als Value
    std::map<uint32_t, unsigned char> mem_map;

    sc_out<bool> finished;     // True, sobald Speicher-Operation zu Ende ist
    sc_out<uint32_t> out_data; // Die Daten die evtl. aus dem Hauptspeicher gelesen werden sollten

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
            // Warten, bis richtige physikalische Adresse zur Verfügung steht
            wait(got_address.posedge_event());
            wait(SC_ZERO_TIME);

            // Zu Beginn ist die Speicheroperation noch nicht fertig
            finished->write(false);

            // Methode, um vom Speicher zu lesen / in den Speicher zu schreiben
            update_memory();

            // Benötigten Zyklen abwarten, um benötigte Zeit zu simulieren
            unsigned int cycle_counter = 0;
            while (cycle_counter < memory_latency)
            {
                cycle_counter++;
                wait();
            }
            wait(SC_ZERO_TIME);

            // Speicheroperation ist nun zu Ende
            finished->write(true);
        }
    }

    void update_memory()
    {
        if (we->read() == true)
        {
            // SCHREIBZUGRIFF

            // Daten byteweise aus dem data-Signal auslesen
            // Zur Übersicht zwischenzeitlich in einem Array speichern
            unsigned char data_bytes[4];
            data_bytes[3] = data->read() & 0xFF;
            data_bytes[2] = (data->read() >> 8) & 0xFF;
            data_bytes[1] = (data->read() >> 16) & 0xFF;
            data_bytes[0] = (data->read() >> 24) & 0xFF;

            // Daten werden von Most Significant Bit bis Least Significant Bit gespeichert
            mem_map[address->read()] = data_bytes[0]; // Most Significant Bit
            mem_map[address->read() + 1] = data_bytes[1];
            mem_map[address->read() + 2] = data_bytes[2];
            mem_map[address->read() + 3] = data_bytes[3]; // Least Significant Bit
        }
        else
        {
            if (mem_map.count(address->read()))
            {
                // LESEZUGRIFF

                // Zur Übersicht, gelesene Werte zwischenzeitlich in dieses Array speichern
                unsigned char data_bytes[4];

                // Daten aus dem Speicher lesen (byteweise)
                data_bytes[0] = mem_map[address->read()];
                data_bytes[1] = mem_map[address->read() + 1];
                data_bytes[2] = mem_map[address->read() + 2];
                data_bytes[3] = mem_map[address->read() + 3];

                // Daten in das out_data-Signal schreiben (byteweise zusammenbauen)
                out_data->write((data_bytes[0] << 24) + (data_bytes[1] << 16) + (data_bytes[2] << 8) + data_bytes[3]);
            }
            else
            {
                // UNGÜLTIGER LESEZUGRIFF

                // falls noch nie in diese Adresse geschrieben wurde, soll 0 als Default-Wert zurückgegeben werden
                out_data->write(0);
            }
        }
    }
};
#endif