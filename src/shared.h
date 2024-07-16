#ifndef SHARED_H
#define SHARED_H

//hint
#define MSG_HINT "Hinweis: Verwenden Sie '-h/--help', um weiter Informationen bezüglich möglicher Argumente und ihrer Benutzung zu erhalten\n"

//             //
//   GENERAL   //
//             //

#define ERR_GENERAL_UNKNOWN "Fehler: Es ist ein unerwarteter Fehler aufgetreten\n"
#define ERR_GENERAL_MEMORY_ALLOCATION_ERROR "Fehler: Es konnte kein Speicher alloziert werden\n"
#define ERR_GENERAL_CANT_OPEN_FILE(file) "Fehler: Die Datei '%s' konnte nicht geöffnet werden\n", file

//                  //
// ARGUMENT STRING  //
//                  //

#define cycle_str "cycles (-c/--cycles)"
#define blocksize_str "blocksize (-b/--blocksize)"
#define v2b_block_offset_str "v2b-block-offset (-o/--v2b-block-offset)"
#define tlb_size_str "tlb-size (-s/--tlb-size)"
#define tlb_latency_str "tlb-latency (-t/--tlb-latency)"
#define memory_latency_str "memory-latency (-m/--memory-latency)"
#define tracefile_str "tracefile (-f/--tf)"



//                  //
// ARGUMENT PARSING //
//                  //

// invalid arguments
#define ERR_UNKNOWN_OPTION "Fehler: Es wurde eine unbekannte Option übergeben; Die Eingabedatei darf nicht mit '-' beginnen\n%s", MSG_HINT
#define ERR_TOO_MANY_OPTION "Fehler: Es wurden zu viele Optionen übergeben\n%s", MSG_HINT
#define ERR_NO_REQUIRED_ARGUMENT(arg) "Fehler: Folgende Option benötigt ein Argument: %s\n%s", arg, MSG_HINT

// illegal arguments
#define ERR_NO_FILE_INPUT "Fehler: Ungültiges Argument: Es muss eine Eingabedatei als positional Argument übergeben werden\n%s", MSG_HINT
#define ERR_ILLEGAL_ARGUMENT_TRACEFILE "Fehler: Ungültiges Argument: 'tracefile (-f/--tf)' es konnte keine Datei mit diesem Namen erstellt werden\n%s", MSG_HINT
#define ERR_ILLEGAL_ARGUMENT_CONVERSION(arg) "Fehler: Ungültiges Argument: Das Argument von '%s' konnte zu keinem Integer umgewandelt werden\n%s", arg, MSG_HINT
#define ERR_ILLEGAL_ARGUMENT(arg,lowerBound,upperBound) "Fehler: Ungültiges Argument: Das Argument von '%s' muss im Interval [%u; %u] liegen\n%s", arg, lowerBound, upperBound,MSG_HINT
#define ERR_AlREADY_SET(arg) "Fehler: '%s' wurde bereits gesetzt\n%s",arg,MSG_HINT
#define ERR_ILLEGAL_TRACEFILE_NAME "Fehler: Ungültiges Argument: Das Argument von 'tracefile (-f/--tf)' darf nicht mit '-' beginnen\n%s",MSG_HINT



#define HELP_MSG \
"////////////////////////////////////////      r5mm:      ////////////////////////////////////////\n"  \
"\n"             \
"Dieses Programm simuliert einen Translation Lookaside Buffer, mögliche Eingabedateien können in dem Verzeichnis examples gefunden werden oder mit dem Javaprogramm erstellt werden\n" \
"\n"                              \
"Optionen:                     Standardwert:                mögliche Argumente:     Beschreibung:\n"     \
"   -c, --cycles               1000000                      [0; 2147483647] int     Gibt an, wie viele Zyklen simuliert werden sollen\n" \
"   -b, --blocksize            4096                         [1; 4294967295] uint    Gibt die Größe eines Speicherblockes in Byte an\n"                         \
"   -o, --v2b-block-offset     4                            [0; 4294967295] uint    Gibt an, um wie viele Blöcke die physische Adresse verschoben wird um die virtuelle Adresse darzustellen\n" \
"   -s, --tlb-size             64                           [0; 4294967295] uint    Gibt, wie viele Einträge der TLB gleichzeitig speichern kann\n" \
"   -t, --tlb-latency          1                            [0; 4294967295] uint    Gibt die Latenzzeit des TLB in Zyklen an\n"                    \
"   -m, --memory-latency       100                          [0; 4294967295] uint    Gibt die Latenzzeit des Hauptspeichers in Zyklen an\n"   \
"   -f, --tf                   null                         <Dateiname>             Name der Ausgabedatei, falls ein Tracefile erstellt werden soll\n"                 \
"   <Dateiname>                !muss immer gesetzt werden!  <Dateiname>             Name der Eingabedatei, mit den zu verarbeitenden Daten\n"\
"   -h, --help                 --------------------------------------------------   Gibt diese Nachricht aus\n" \
"   -q, --quickStart           --------------------------------------------------   Setzt und überschreibt alle Parameter wie aus der folgenden beispiel Eingabe\n"\
"\n"             \
"Eine mögliche valide Eingabe wäre: ./r5mm -c 2000 --blocksize 16 --tlb-size=16 -t 2 --tf tracefile examples/kurze_Eingabedatei_valid.csv"                 \
"\n"\
"Dieses Programm ist die Projektabgabe für das Fach 'Grundlagenpraktikum: Rechnerarchitektur', Projektaufgabe A14\n"                                                                   \
"////////////////////////////////////////      r5mm:      ////////////////////////////////////////\n"

//                      //
// REQUEST FILE PARSING //
//                      //

#define ERR_FILE_PREMATURE_END_OF_FILE(arg, line) "Fehler: Request-Datei ungültig: Zeile %zu - Erwartet wurde '%s', jedoch Ende der Datei gefunden\n", line, arg
#define ERR_FILE_PREMATURE_NEW_LINE(arg, line) "Fehler: Request-Datei ungültig: Zeile %zu - Erwartet wurde '%s', jedoch neue Zeile gefunden\n", line, arg
#define ERR_FILE_WHITE_SPACE(line) "Fehler: Request-Datei ungültig: Zeile %zu - Leerzeichen sind nicht erlaubt\n", line
#define ERR_FILE_INVALID_ARG_RW(line) "Fehler: Request-Datei ungültig: Zeile %zu - 'write_enable' muss einer der Werte [r,w,R,W] sein\n", line
#define ERR_FILE_INVALID_ARG_ADDR(line) "Fehler: Request-Datei ungültig: Zeile %zu - 'address' muss ein Integer im Interval [0; 4294967295] sein\n", line
#define ERR_FILE_INVALID_ARG_DATA(line) "Fehler: Request-Datei ungültig: Zeile %zu - 'write_data' muss ein Integer im Interval [0; 4294967295] sein\n", line

#define ERR_FILE_TOO_MANY_ARGS(line) "Fehler: Request-Datei ungültig: Zeile %zu - Erwartet wurde neue Request, jedoch weiteres Argument gefunden\n", line


//                //
// GLOBAL STRUCTS //
//                //

typedef struct Request
{
    uint32_t addr;
    uint32_t data;
    int we;
} Request;

typedef struct Result
{
    size_t cycles;
    size_t misses;
    size_t hits;
    size_t primitive_gate_count;
} Result;


#endif //SHARED_H
