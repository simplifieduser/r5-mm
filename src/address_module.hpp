#ifndef ADDRESS_MODULE_HPP
#define ADDRESS_MODULE_HPP

#include <systemc>
#include <unordered_set>
using namespace sc_core;

SC_MODULE(ADDRESS_GETTER)
{
    // Input
    sc_in<bool> clk;
    sc_in<uint32_t> virtual_address;
    sc_in<bool> start; // True, sobald die nächste Übersetzung stattfinden kann

    std::vector<uint32_t> buffer; // Enthält alle aktuell im TLB gespeicherten Tags

    std::unordered_set<uint32_t> previuosly_visited; // Enthält alle bisher gespeicherten Tags, um Cold Misses zu fangen

    // Eigenschaften des TLB
    unsigned tlbs_latency,
        memory_latency, blocksize, v2b_block_offset, tlb_size;

    sc_out<bool> finished; // True, sobald die richtige physikalische Adresse am physical_address-Signal liegt
    sc_out<uint32_t> physical_address;

    sc_out<int> cycles; // Zählt, wie viele Zyklen gebraucht wurden, um Addresse zu übersetzen

    sc_out<bool> hit; // True, wenn Addresse im TLB abgespeichert war, sonst False

    int latency; // wird auf tlb_latency oder memory_latency gesetzt, je nachdem ob es ein Hit oder Miss war

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
            // Auf Startsignal warten
            wait(start.posedge_event());
            wait(SC_ZERO_TIME);

            // Zu Beginn der Übersetzung ist diese noch nicht fertig -> finished auf False setzen
            finished->write(false);

            // Methode, um physikalische Adresse zu berechnen und richtige latency zu bestimmen
            set_values();

            // Benötigten Zyklen abwarten, um benötigte Zeit zu simulieren
            int cycle_count = 0;
            while (cycle_count < latency)
            {
                cycle_count++;
                cycles->write(cycle_count);
                wait();
            }
            wait(SC_ZERO_TIME);

            // Endgültige Anzahl von gebrauchten Zyklen
            cycles->write(cycle_count);
            // Übersetzung ist nun fertig, Adresse an physical_address-Signal darf benutzt werden
            finished->write(true);
        }
    }

    void set_values()
    {
        // Tag der virtuellen Adresse berechnen -> Rechtsshift der virtuellen Adresse um blocksize
        uint32_t tag = virtual_address->read() >> (int)std::log2(blocksize);

        // Überprüfen, ob tlb_size = 0
        if (tlb_size == 0)
        {
            // Automatisch ein Miss, da nichts im TLB gespeichert werden kann
            latency = tlbs_latency + memory_latency;
            hit->write(false);
        }
        else
        {
            // Aktuellen tag im TLB holen
            uint32_t current_tag_in_tlb = buffer[tag % tlb_size];

            // Überprüfen, ob die Tags übereinstimmen (ob Tag aktuell im TLB gespeichert) und der Tag tatsächlich schonmal gecached wurde (kein Cold Miss)
            if (tag == current_tag_in_tlb && previuosly_visited.count(tag))
            {
                // HIT

                // Tag als schonmal gecached markieren
                previuosly_visited.insert(tag);

                latency = tlbs_latency;
                hit->write(true);
            }
            else
            {
                // MISS

                // Tag als schonmal gecached markieren
                previuosly_visited.insert(tag);

                // richtigen Tag in den Buffer laden (dieser ist nun im TLB gespeichert)
                buffer[tag % tlb_size] = tag;

                latency = tlbs_latency + memory_latency;
                hit->write(false);
            }
        }

        // physikalische Addresse konstruieren (Abstraktion)
        // oberen Bits aus dem TLB rauslesen (v2b_block_offset darauf addieren) und unteren Bits aus der virtuellen Adresse kopieren
        if (blocksize > 1)
        {
            physical_address->write(((tag + v2b_block_offset) << (int)std::log2(blocksize)) + (virtual_address->read() % (int)std::log2(blocksize)));
        }
        else
        {
            // Falls blocksize < 2 ist der Tag genauso lang wie die Adresse
            physical_address->write(tag + v2b_block_offset);
        }
    }
};
#endif