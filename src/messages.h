#ifndef IO_MESSAGES_H
#define IO_MESSAGES_H

#endif //IO_MESSAGES_H
#include <string.h>
#include "stdint.h"
#include <stdio.h>
//Here are messages to be returned
#define INT_MAX "2147483647"
#define UINT_MAX "4294967295"

//hint
#define HINT "Hinweis: Verwenden Sie '-h/--help', um weiter Informationen bezüglich möglicher Argumente und ihrer Benutzung zu erhalten"

//error msg input
#define ILLEGAL_ARGUMENT_PASSING "Fehler: ungültiges Argument: "
#define UNKNOWN_OPTION "Fehler: es wurde eine unbekannte Option übergeben, bitte entferne: "
#define TOO_MANY_OPTION "Fehler: es wurden zu viele Optionen übergeben"
#define NO_REQUIRED_ARGUMENT "Fehler: folgende Option benötigt ein Argument: "

//ILLEGAL_ARGUMENT_PASSING
#define NO_FILE_INPUT ILLEGAL_ARGUMENT_PASSING "Es muss eine Eingabedatei als Positional Argument übergeben werden"
#define ILLEGAL_ARGUMENT_CYCLES ILLEGAL_ARGUMENT_PASSING "'cycles (-c/--cycles)' muss ein Integer im Interval [0; " INT_MAX "] sein"
#define ILLEGAL_ARGUMENT_BLOCKSIZE ILLEGAL_ARGUMENT_PASSING  "'blocksize (-b/--blocksize)' muss ein Integer im Interval [1; " UINT_MAX "] sein"
#define ILLEGAL_ARGUMENT_V2B_BLOCK_OFFSET ILLEGAL_ARGUMENT_PASSING  "'v2b-block-offset (-o/--v2b-block-offset)' muss ein Integer im Interval [0; " UINT_MAX "] sein"
#define ILLEGAL_ARGUMENT_TLB_SIZE ILLEGAL_ARGUMENT_PASSING  "'tlb-size (-s/--tlb-size)' muss ein Integer im Interval [0; " UINT_MAX "] sein"
#define ILLEGAL_ARGUMENT_TLB_LATENCY ILLEGAL_ARGUMENT_PASSING  "'tlb-latency (-t/--tlb-latency)' muss ein Integer im Interval [0; " UINT_MAX "] sein"
#define ILLEGAL_ARGUMENT_MEMORY_LATENCY ILLEGAL_ARGUMENT_PASSING  "'memory-latency (-m/--memory-latency)' muss ein Integer im Interval [0; " UINT_MAX "] sein"
#define ILLEGAL_ARGUMENT_TRACEFILE ILLEGAL_ARGUMENT_PASSING "'Tracefile (-f/--tf)' es konnte keine Datei mit diesem Namen erstellt werden"



#define HELP_MSG \
"Verwendung: r5mm\n"  \
"\n"             \
"Beschreibung: -----TO ´°_°` BE CONTINUED\n" \
"Optionen:                     Standardwert:                mögliche Argumente:     Beschreibung:\n"     \
"   -c, --cycles               10000                        [0; 2147483647] int     Gibt an, wie viele Zyklen simuliert werden sollen\n" \
"   -b, --blocksize            4                            [1; 4294967295] uint    Gibt die Größe eines Speicherblockes in Byte an\n"                         \
"   -o, --v2b-block-offset     4                            [0; 4294967295] uint    Gibt an, um wie viele Blöcke die physische Adresse verschoben wird um die virtuelle Adresse darzustellen\n" \
"   -s, --tlb-size             64                           [0; 4294967295] uint    Gibt, wie viele Einträge der TLB gleichzeitig speichern kann\n" \
"   -t, --tlb-latency          1                            [0; 4294967295] uint    Gibt die Latenzzeit des TLB in Zyklen an\n"                    \
"   -m, --memory-latency       100                          [0; 4294967295] uint    Gibt die Latenzzeit des Hauptspeichers in Zyklen an\n"   \
"   -f, --tf                   null                         <Dateiname>             Name der Ausgabedatei, falls ein Tracefile erstellt werden soll\n"                 \
"   <Dateiname>                !muss immer gesetzt werden!  <Dateiname>             Name der Eingabedatei, mit den zu verarbeitenden Daten\n"\
"   -h, --help                --------------------------------------------------    Gibt diese Nachricht aus\n"
