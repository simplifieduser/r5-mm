# Robert-5 Memory Mapper

> Wie man die Simulation erstellen und ausführen kann, wird in [USAGE](USAGE.md) erklärt.

## Recherche-Ergebnisse

Die Aufgabe bestand darin, einen Translation Lookaside Buffer (TLB) zu simulieren und damit das Speicherzugriffsverhalten einer Summe über einer verketteten Liste zu untersuchen.

Üblicherweise hat eine TLB zwischen 16 und 512 Einträge, wobei ein geladener Block 4096 Byte groß ist. Dabei liegt die Hit-Rate zwischen 99% und 99,9% und die Zugriffszeit beträgt ca. 1 Zyklus. Eine übliche Zugriffszeit zum Hauptspeicher liegt bei ca. 100 Zyklen.

Um die Effizienz und Ausführungszeit des TLBs zu untersuchen, haben wir die Größe des TLBs, sowie die Anordnung von 102400 Daten variiert. Ohne TLB liegt die Zugriffszeit auf diese Daten bei nahezu 21 Millionen Zyklen.

Bei der optimalen Verteilung haben wir die Daten auf 100 Seiten mit je 1024 Einträgen in chronologischer Reihenfolge verteilt. Hierbei ist schon bei einer TLB-Größe von 1 die Zyklenanzahl von ca. 10 Millionen erreicht, um auf diese Daten zuzugreifen. Auch bei größeren TLBs verändert sich diese Zahl nicht, da bereits bei einer Größe von 1 die Hit-Rate bei 100% liegt. Dieses Szenario ist jedoch unrealistisch.

Für eine etwas realistischere mittlere Verteilung haben wir 1024 Seiten, mit je 100 Einträgen untersucht. Dabei ist die benötigte Zyklenanzahl bei einer TLB-Größe von 1 bei ca. 12 Millionen, bei einer Größe von 512 bei ca. 11 Millionen, was zeigt, dass ein größerer TLB die Performanz verbessern kann. Die prozentuale Verbesserung durch den TLB ist bei wachsender Größe von 43% auf 46% gestiegen. Dass die Hit-Rate auch bei einer weniger optimalen Verteilung bereits über 85% liegt, zeigt, wie essentiell der TLB für die Verbesserung der Performanz ist.

Die schlecht möglichste Verteilung der Daten, die mit einem Eintrag pro Seite auf 102400 Seiten verteilt extrem unwahrscheinlich ist, profitiert nicht von dem TLB, denn es kommt immer zu einem Cache-Miss, und braucht somit unabhängig von der Cache-Größe immer nahezu 21 Millionen Zyklen.

Im Allgemeinen kann man klar feststellen, dass bereits bei einer sehr kleinen TLB-Größe die Zugriffszeit auf die Daten durch den TLB erheblich verringert wird. Dadurch wird erkenntlich, wie wichtig ein solcher Translation Lookaside Buffer ist, um die Performanz von Speicherzugriffen zu verbessern.

## Beitrag der Gruppenmitglieder

### Elena Reinbold Fraire

- Logik der Simulation
- Implementierung der Simulation in SystemC
- Readme und Usage

### Jonah Zabel

- Makefile
- Request-File-Parsing
- Tests für file_parser und Simulation
- Projekt Management

### Lukas Wolf

- CLI
- Tests für cli_parser
- Recherche
