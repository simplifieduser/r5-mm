# Robert-5 Memory Mapper

> Wie man die Simulation erstellen und ausführen kann, wird in [USAGE](USAGE.md) erklärt.

## Recherche-Ergebnisse

In modernen Prozessoren ist eine kurze Zugriffszeit auf Daten, welche im Hauptspeicher gespeichert sind, essenziell. Um
die Übersetzungszeit zwischen virtuellen und physischen Seiten zu beschleunigen, kann ein Translation Lookaside Buffer (
TLB) verwendet werden. Dieser speichert als Mengen assoziativer Cache 16 - 512 Adresspaare, welche eine Referenz auf
eine 4 KiB Seite im Hauptspeicher sind. Durch Hit-Raten von über 99 % und einer geringen Zugriffszeit von einem Zyklus
ist der TLB erheblich schneller als ein Hauptspeicherzugriff, welcher über 100 Zyklen benötigt.

In unserem Fallbeispiel wurde anhand eines direkt abbildenden TLB das Speicherzugriffsverhalten einer Summe über eine
verkettete Liste bei zunehmender Größe des TLB untersucht. Hierbei wurden 102400 Integer über den Hauptspeicher verteilt
und sowohl die Ausführungszeit als auch die Hit-Rate beim Zugriff auf diese Daten gemessen.

Bei optimaler Verteilung befinden sich die Daten chronologisch auf 100 Seiten im Hauptspeicher. Hierbei wird bereits bei
einer TLB-Größe von 1 eine Hit-Rate von 99,9 % erreicht und die Simulationszeit um fast 50 % gesenkt. Mit zunehmender
Größe des TLB bleiben diese Werte jedoch unverändert.

Da solch eine perfekte Verteilung nur sehr schwierig umzusetzen ist, wurden weitere Szenarien mit nicht chronologischer
Datenverteilung auf 128, 512 und 1024 Seiten untersucht. Bei all diesen Szenarien konnte bereits ein TLB der Größe 1 die
Simulationszeit um 43 % bis 48 % senken, wobei Hit-Raten von 87 % bis 97 % erreicht wurden. Jedoch fiel auf, dass sich
bei zunehmender Größe des TLB die Simulationszeit und die Hit-Raten nur kaum verbesserten. Erst ab 64 Speicherzellen
nahmen die Hit-Raten erneut stark zu und erreichten gegen Ende sowohl bei der Versuchsreihe mit 128 als auch bei der mit
512 Seiten 99,9 %.

Lediglich die Versuchsreihe mit 1024 Seiten konnte selbst bei einer TLB-Größe von 512 nur eine Hit-Rate von 93 %
erreichen.

Sollten die Daten sehr schlecht im Speicher verteilt sein, ein Datum pro Seite, dann hätte der TLB eine negative
Auswirkung auf die Laufzeit (vgl. Diagramme in den [PDF-Slides](slides/slides.pdf)).

Es zeigt sich, dass bereits ein sehr kleiner TLB bei gutem Speichermanagement die Zugriffszeit auf die Daten erheblich
verringert.

## Beitrag der Gruppenmitglieder

### Elena Reinbold Fraire

* Implementierung der Simulationslogik des TLB in SystemC
* Ausarbeitung der Readme und Usage Datei

### Jonah Zabel

* Erstellung der Makefile, des Git-Repositories und des Request-File-Parsers
* Entwicklung mehrere Fuzzer zur Projekttestung

### Lukas Wolf

* Erstellung des Programms zur Eingabeverarbeitung
* Ausarbeitung des Theorieteiles mit Entwicklung eines Javatools zur Erstellung der
  Versuchsdateien [Javatool](examples/Javatool/src/Main.java)
