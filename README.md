# Nearr - Silnik Szachowy C++

Silnik szachowy wykorzystujący technikę **Bitboards** do reprezentacji stanu planszy i manipulacji danymi.

---

## 🎯 Status Projektu

Projekt w fazie implementacji **ewaluacji pozycji**. Podstawowe mechaniki poruszania się figur (w tym roszada, en passant i promocja) są zaimplementowane i działają w ramach funkcji `make_move`.

## ⚙️ Kluczowe Technologie

* **Język:** C++ (Standard 17/20).
* **Struktura planszy:** Bitboards (`uint64_t`).
* **Format pozycji:** Obsługa notacji FEN.

## 🛠️ Zaimplementowane Elementy

### I. Stan Planszy i Logika
* **Parser FEN:** Wczytywanie pozycji, tury, praw roszady i pola en passant.
* **Zarządzanie stanem:** Wykonywanie ruchów (`make_move`) z poprawną aktualizacją masek bitowych dla wszystkich figur.
* **Prawa roszady:** Funkcja `update_castling_rights` automatycznie koryguje uprawnienia po ruchach króla, wież lub zbiciach.

### II. Obsługa Ruchów
* **Ruchy specjalne:** Pełna obsługa roszady, bicia w przelocie (En Passant) oraz promocji piona.
* **Generowanie ataków:** Wstępnie wygenerowane tablice ataków dla pionów i króla do weryfikacji pól zagrożonych.

### III. Ewaluacja (W trakcie)
* **Tapered Evaluation:** Przygotowanie podziału oceny na fazy gry (Midgame / Endgame).
* **PST (Piece-Square Tables):** Wdrażanie wartości pól dla poszczególnych typów figur.
* **Struktura pionów:** Planowana detekcja słabości (piony izolowane i zdublowane).

## 🚀 Plan Rozwoju
1. Zakończenie logiki oceniania pozycji (Ewaluacja).
2. Implementacja algorytmu przeszukiwania drzewa ruchów (Search).
3. Dodanie protokołu komunikacji UCI.
