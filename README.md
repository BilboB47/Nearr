# Nearr 1.1 - Silnik Szachowy C++

Nearr to silnik szachowy oparty na architekturze bitowej (**Bitboards**), zaprojektowany z myślą o wysokiej wydajności i głębokim przeszukiwaniu drzewa wariantów.

---

## 🖼️ Podgląd Rozgrywki

![Interfejs Silnika Nearr](https://via.placeholder.com/800x400.png?text=Wstaw+Tutaj+Swoj+Zrzut+Ekranu)  
*(Wskazówka: Wrzuć screena z konsoli do folderu z projektem i podmień link powyżej)*

<<<<<<< HEAD
=======
| Ruch Gracza | Ruch Silnika (Bot) |
| :---: | :---: |
| ![Ruch Gracza](Images/Player.png) | ![Ruch Bota](Images/Bot.png) |

>>>>>>> cc0967da09e6e1479602f2b8a021b9e6ae918f87
---

## ⚙️ Kluczowe Funkcjonalności

### I. Reprezentacja i Generowanie Ruchów
* **Bitboards:** Wykorzystanie 64-bitowych masek do błyskawicznych operacji na stanach planszy.
* **Magic Bitboards:** Zaawansowana technika generowania ruchów figur dalekosiężnych (Wieże, Gońce, Hetmany) za pomocą predefiniowanych tabel "magicznych".
* **Zobrist Hashing:** Implementacja unikalnych kluczy pozycji, pozwalająca na błyskawiczne operacje na Tablicy Transpozycji oraz detekcję powtórzeń (reguła 3-krotnego powtórzenia).

### II. Silnik Szukający (Search)
* **Alpha-Beta Pruning:** Klasyczny algorytm optymalizacji przeszukiwania, redukujący liczbę analizowanych gałęzi.
* **Iterative Deepening:** Strategia szukania warstwowego, umożliwiająca lepsze sortowanie ruchów i kontrolę czasu.
* **Transposition Table (TT):** Globalna tablica mieszająca przechowująca wyniki (Score, Depth, Flags: Exact/Alpha/Beta). Pozwala uniknąć ponownego przeliczania tych samych pozycji.
* **Quiescence Search:** Specjalna faza szukania "ciszy" na końcach gałęzi, eliminująca błędy wynikające z tzw. efektu horyzontu (kontynuacja analizy bić).
* **Killer Heuristic:** Zapamiętywanie "morderczych ruchów", które spowodowały odcięcia w innych gałęziach, co drastycznie przyspiesza proces szukania.

### III. Ewaluacja i Taktyka
* **Tapered Evaluation:** Dynamiczne przechodzenie między fazą gry środkowej a końcówką.
* **PST (Piece-Square Tables):** Pozycjonowanie figur w oparciu o ich strategiczne umiejscowienie na szachownicy.
* **Mate Distance Pruning:** Logika dążąca do zadania mata w jak najmniejszej liczbie posunięć (korekta punktacji względem głębokości `ply`).

---

## 🎮 Instrukcja Obsługi (Tryb Konsolowy)

### Format ruchu:
Wpisujemy pola w notacji algebraicznej, np. `e2e4`. 
* **Roszada:** Przesuwamy króla o dwa pola, np. `e1g1`.
* **Promocja:** Pionek promowany jest automatycznie na **Hetmana (Q)**.

### Legenda Symboli:
* **Białe (WIELKIE LITERY):** K, Q, R, B, N, I (Pionek)
* **Czarne (małe litery):** k, q, r, b, n, i (pionek)

---

## 🛠️ Technologie i Standardy
* **Język:** C++ (Standard 17/20)
* **Optymalizacje sprzętowe:** Wykorzystanie instrukcji procesora (intrinsics: `_BitScanForward64`, `__popcnt64`).
* **Formaty:** Pełna obsługa notacji FEN.

## 🚀 Plan Rozwoju
- [ ] Implementacja protokołu **UCI** (Universal Chess Interface).
- [ ] **Null Move Pruning:** Przyspieszenie szukania poprzez symulację "oddania ruchu".
- [ ] **Zarządzanie Czasem:** Logika dostosowująca czas namysłu do pozostałego czasu na zegarze.

---

### Jak uruchomić?
1. Skompiluj projekt za pomocą MSVC (Visual Studio) lub GCC/Clang.
2. Uruchom plik binarny.
3. Postępuj zgodnie z instrukcjami w konsoli, aby rozpocząć partię!
