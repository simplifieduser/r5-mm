#ifndef MODULES_HPP
#define MODULES_HPP

#include <systemc>
#include "shared.h"
#include "address_module.hpp"
#include "memory_module.hpp"
using namespace sc_core;

extern "C" Result run_simulation(int cycles, unsigned tlbSize, unsigned tlbsLatency, unsigned blocksize, unsigned v2bBlockOffset, unsigned memoryLatency, size_t numRequests, Request *requests, const char *tracefile);

SC_MODULE(REQUEST_PROCESSOR)
{
    sc_in<bool> clk;

    // Übergebenen Eigenschaften des TLB
    unsigned tlb_size, tlbs_latency, blocksize, v2b_block_offset, memory_latency;
    size_t num_requests;
    Request *requests;

    ADDRESS_GETTER address_getter; // Holt die richtige physikalische Adresse
    MAIN_MEMORY data_manager;      // Schreibt bzw. liest die Daten in bzw. von dem Hauptspeicher
    std::vector<uint32_t> buffer;  // Speichert, welche Adresse aktuell im TLB gespeichert sind

    // Hilfsignale, um Module miteinander zu verbinden
    sc_signal<uint32_t> current_virt_address, current_phys_address, data, out_data;
    sc_signal<int> addr_cycles;
    sc_signal<bool> get_address_finished, manage_data_finished, we, start_request, hit, notify_memory;

    // Output
    sc_out<size_t> misses, primitive_gate_count, hits;
    sc_out<int> cycles;

    SC_CTOR(REQUEST_PROCESSOR);
    REQUEST_PROCESSOR(sc_module_name name, unsigned tlb_size, unsigned tlbs_latency, unsigned blocksize, unsigned v2b_block_offset, unsigned memory_latency, size_t num_requests, Request *requests, std::vector<uint32_t> buffer_param) : sc_module(name), tlb_size(tlb_size), tlbs_latency(tlbs_latency), blocksize(blocksize), v2b_block_offset(v2b_block_offset), memory_latency(memory_latency), num_requests(num_requests), requests(requests), address_getter("address_getter", buffer_param, tlb_size, tlbs_latency, blocksize, v2b_block_offset, memory_latency), data_manager("data_manager", memory_latency), buffer(buffer_param)
    {
        // Alle input und output-ports binden

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

        // Erstmal soll kein Modul etwas machen; beide Startsignale auf false setzen
        start_request.write(false);
        notify_memory.write(false);

        SC_THREAD(run_program);
        sensitive << clk.pos();
    }

    void run_program()
    {

        // Anzahl von primitiven Gattern setzen:
        // Tag-Größe + Adressen-Größe + 1 -> Anzahl Bits pro Zeile (+1: speichert, ob Zeile schon einmal benutzt wurde)
        // Anzahl Bits pro Zeile * Anzahl Zeilen (= tlb_size)
        // Anzahl Bits * 4 -> 4 Gatter benötigt, um 1 Bit zu speichern
        unsigned int count = (2 * ((32 - (int)std::log2(blocksize))) + 1) * 4 * tlb_size;

        // Überprüfen, ob überhaupt zwei Tags verglichen werden können
        if (tlb_size > 0)
        {
            // Gatter, die man zum vergleichen zweier Tags braucht (Abschätzung für Subtraktion)
            // 182 = 150 für die Addition von 2 32-Bit Zahlen + 32 XOR-Gatter für das Zweierkomplement
            primitive_gate_count->write(count + 182);
        }
        else
        {
            primitive_gate_count->write(0);
        }

        // Sicherstellen, dass diese Werte mit 0 initialisiert sind
        cycles->write(0);
        hits->write(0);
        misses->write(0);

        for (size_t i = 0; i < num_requests; i++)
        {
            wait(SC_ZERO_TIME);

            // Signale mit Werten des aktuellen Requests setzen
            data.write(requests[i].data);
            we.write(requests[i].we);
            current_virt_address.write(requests[i].addr);

            // address_getter starten
            start_request.write(true);

            // Auf richtige Adresse warten, die durch adress_getter geholt wird
            wait(get_address_finished.posedge_event());
            wait(SC_ZERO_TIME);

            // Output mit zurückgegebenen Werten aktualisieren, falls tlb vorhanden ist
            if (tlb_size > 0)
            {
                cycles->write(cycles->read() + address_getter.cycles->read());
                hits->write(hits->read() + address_getter.hit->read());
                misses->write(misses->read() + !address_getter.hit->read());
            }
            else
            {
                // Wenn tlb_size = 0, dann existiert der TLB nicht, dessen latency wird also nicht gezählt und es kann keine Hits oder Misses geben
                // Da jedes Mal die physische Adresse geholt werden muss, wird diese latency immer dazuaddiert
                cycles->write(cycles->read() + memory_latency);
            }

            // address_getter soll jetzt bis zum nächsten Request warten
            start_request.write(false);

            // Speicheroperation kann jetzt stattfinden
            notify_memory.write(true);

            // Warten, bis Speicheroperation zu ende ist
            wait(manage_data_finished.posedge_event());
            wait(SC_ZERO_TIME);

            // data_manager soll jetzt bis zum nächsten Request warten
            notify_memory.write(false);

            // Zyklen (nochmal) aktualisieren
            cycles->write(cycles->read() + memory_latency);
            wait();
        }
        sc_stop();
    }
};
#endif
