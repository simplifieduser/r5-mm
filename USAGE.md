# Ausführung der Simulation

## Anforderungen
* C-Version: 17
* C++-Version: 14
* SystemC-Version: 2.3.3

Die Makefile verwendet standardmäßig clang++ und clang. Möchte man einen anderen Compiler verwenden, muss dies in der Makefile geändert werden.
Zudem muss die SystemC-Bibliothek installiert sein und die Umgebungsvariable `SYSTEMC_HOME` auf den Pfad zur SystemC-Bibliothek gesetzt sein.

## Erstellen
> Die `SYSTEMC_HOME`-Umgebungsvariable muss auf den Pfad zur SystemC-Bibliothek gesetzt sein, damit das Programm erstellt werden kann.

Die mitgelieferte Makefile kann verwendet werden, um das Programm zu erstellen. Dazu wird folgender Befehl ausgeführt:
```bash
make
```
Alternativ stehen folgende Ziele zur Verfügung:
* `release` erstellt das Programm mit den Standard-Optimierungen. _Dies ist das Standard-Ziel._
* `debug` erstellt das Programm mit Debug-Informationen.
* `clean` löscht alle erstellten Dateien.

## Verwendung

> Da die SystemC-Bibliothek extern referenziert wird, muss diese weiterhin an unter dem zur Kompilierung verwendeten Pfad verfügbar sein.

Nachdem das Program erstellt wurde, kann es mit folgendem Befehl ausgeführt werden:
```bash
./r5-mm [Optionen] <Dateiname>
```

### Quickstart & Hilfe
Um die Simulation mit voreingestellten Werten zu starten, kann folgender Befehl verwendet werden:
```bash
./r5-mm -q
```
Alternativ kann auch die Hilfe aufgerufen werden:
```bash
./r5-mm -h
```

### Programm Argumente
Es können folgende Simulations-Optionen übergeben werden:

| Optionen               | Standardwert                  | Mögliche Argumente   | Beschreibung                                                                                             |
|------------------------|-------------------------------|----------------------|----------------------------------------------------------------------------------------------------------|
| -c, --cylces           | 1000000                       | [0; 2147483647] int  | Gibt an, wie viele Zyklen simuliert werden sollen                                                        |
| -b, --blocksize        | 4096                          | [1; 4294967295] uint | Gibt die Größe eines Speicherblockes in Byte an                                                          |
| -o, --v2b-block-offset | 4                             | [0; 4294967295] uint | Gibt an, um wie viele Blöcke die physische Adresse verschoben wird um die virtuelle Adresse darzustellen |
| -s, --tlb-size         | 64                            | [0; 4294967295] uint | Gibt an, wie viele Einträge der TLB gleichzeitig speichern kann                                             |
| -t, --tlb-latency      | 1                             | [0; 4294967295] uint | Gibt die Latenzzeit des TLB in Zyklen an                                                                 |
| -m, --memory-latency   | 100                           | [0; 4294967295] uint | Gibt die Latenzzeit des Hauptspeichers in Zyklen an                                                      |
| -f, --tf               | null                          | &lt;Dateiname&gt;    | Name der Ausgabedatei, falls ein Tracefile erstellt werden soll                                          |
| &lt;Dateiname&gt;      | **muss immer gesetzt werden** | &lt;Dateiname&gt;    | Name der Eingabedatei, mit den zu verarbeitenden Daten                                                   |

### Request-Datei

> Leerzeichen sind in der Request-Datei nicht erlaubt und führen zu einem Fehler.

Um die Simulation starten zu können, muss eine CSV-Datei als Argument übergeben werden.
Diese Datei enthält die Speicherzugriffe, die simuliert werden sollen.
Die Datei muss folgendes Format haben:

| Operation    | Erstes Argument (Addresse) | Zweites Argument (Daten)   |
|--------------|----------------------------|----------------------------|
| `r` oder `R` | vorzeichenlose 32-Bit-Zahl |                            |
| `w` oder `W` | vorzeichenlose 32-Bit-Zahl | vorzeichenlose 32-Bit-Zahl |

Als Trennzeichen wird ein `,` (Komma) verwendet. Als Zeilenumbruch werden sowohl `\n`, `\r` als auch `\r\n` unterstützt.

Einige Beispiel-Dateien lassen sich in [Examples](examples) finden.

