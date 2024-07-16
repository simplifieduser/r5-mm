# Ausführung der Simulation

## Anforderungen

- clang, clang++ installiert
- SystemC library von C++ installiert
- C-Version: 17
- C++-Version: 14
- SystemC-Version: 2.3.3


## Verwendung

- Sicherstellen, dass die environmental Variable SYSTEMC_HOME auf den korrekten Pfad zur SystemC library gesetzt ist.
- Für den Build immer zuerst `make release` ausführen.
- Um das Programm mit beispielhaften Werten zu testen, `./r5mm -q` oder `./r5mm --quickStart` ausführen.
- Für genauere Details zu den Parametern und weiteren Optionen '-h / --help' verwenden.

## Optionen und Argumente
| Optionen                  | Standardwert                  | Mögliche Argumente    | Beschreibung      |
| -------------             | -------------                 | ------------------    | ----------------- |
| -c, --cylces              | 1000000                       | [0; 2147483647] int   | Gibt an, wie viele Zyklen simuliert werden sollen |
| -b, --blocksize           | 4096                          | [1; 4294967295] uint  | Gibt die Größe eines Speicherblockes in Byte an |
| -o, --v2b-block-offset    | 4                             | [0; 4294967295] uint  |  Gibt an, um wie viele Blöcke die physische Adresse verschoben wird um die virtuelle Adresse darzustellen |
| -s, --tlb-size            | 64                            | [0; 4294967295] uint  | Gibt, wie viele Einträge der TLB gleichzeitig speichern kann |
| -t, --tlb-latency         | 1                             | [0; 4294967295] uint  | Gibt die Latenzzeit des TLB in Zyklen an |
| -m, --memory-latency      | 100                           | [0; 4294967295] uint  | Gibt die Latenzzeit des Hauptspeichers in Zyklen an |
| -f, --tf                  | null                          | &lt;Dateiname&gt;     | Name der Ausgabedatei, falls ein Tracefile erstellt werden soll |
| &lt;Dateiname&gt;         | !muss immer gesetzt werden!   | &lt;Dateiname&gt;     | Name der Eingabedatei, mit den zu verarbeitenden Daten
