(For international users: this is at least Polish software,
so you would need to know Polish language anyway.)

*Ta wersja zostala zmodyfikowana pod czat polfan.pl
w styczniu roku 2025 przez uzytkownika Polfanu -- DMB.

Kompilacja
==========

Na wspolczesnym systemie Linux teoretycznie powinno
wystarczyc "make", oczywiscie po zainstalowaniu
odpowiednich paczek devel.

Potrzebne biblioteki
====================
* libboost
* ncursesw (ew. BSD curses).

Kompilator
==========
gcc (g++).

Linux
=====
make
make strip

Pod NetBSD
==========
make -f Makefile-NetBSD
make strip

(I analogicznie co do innych Makefile's).

Binarki
=======
W katalogu "binaries" sa gotowe binarki (uwaga: nie wszystkie
byly testowane).

*//DMB, styczen 2025
