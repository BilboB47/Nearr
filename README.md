# Nearr - Zaawansowany Silnik Szachowy C++

Nearr to silnik szachowy oparty na architekturze bitowej (**Bitboards**), zaprojektowany z myślą o wysokiej wydajności i precyzji obliczeń. Silnik posiada w pełni grywalny interfejs konsolowy, pozwalający na partię człowieka z komputerem.

---

## 🖼️ Podgląd Rozgrywki

![Interfejs Silnika Nearr](https://via.placeholder.com/800x400.png?text=Wstaw+Tutaj+Swoj+Zrzut+Ekranu)  
*(Wskazówka: Wrzuć screena z konsoli do folderu z projektem i podmień link powyżej)*

---

## ⚙️ Kluczowe Funkcjonalności

### I. Reprezentacja i Generowanie Ruchów
* **Bitboards:** Wykorzystanie 64-bitowych masek do błyskawicznych operacji na stanach planszy.
* **Magic Bitboards:** Implementacja techniki "Magic" do generowania ruchów figur dalekosiężnych (Wieże, Gońce, Hetmany). Pozwala to na natychmiastowe sprawdzanie blokad i linii ataku bez iterowania po polach.
* **Zobrist Hashing:** Unikalne klucze pozycji generowane za pomocą tablicy Zobrista, umożliwiające detekcję powtórzeń pozycji oraz optymalizację wyszukiwania.

### II. Silnik Szukający (Search)
* **Alpha-Beta Pruning:** Optymalny algorytm przeszukiwania drzewa wariantów, który drastycznie redukuje liczbę analizowanych pozycji poprzez ucinanie nieistotnych gałęzi.
* **Move Ordering:** Inteligentne sortowanie ruchów (np. bicia figur, PV-move), co zwiększa efektywność ucięć Beta w algorytmie Alpha-Beta.
* **Obsługa Końca Gry:** Precyzyjne rozpoznawanie mata (z priorytetem najkrótszej drogi) oraz pata.

### III. Zaawansowana Ewaluacja
* **Tapered Evaluation:** Płynne przechodzenie oceny między fazą gry środkowej (**Midgame**) a końcówką (**Endgame**) w zależności od materiału na planszy.
* **PST (Piece-Square Tables):** Ocena pozycji figur na podstawie dedykowanych tabel wartości (premiowanie skoczków w centrum, krola za obroną pionową itp.).
* **Material Weights:** Klasyczna wycena figur skorelowana z fazą partii.

---

## 🎮 Instrukcja Obsługi (Tryb Konsolowy)

Po uruchomieniu programu wyświetli się instrukcja, a następnie możliwość wyboru koloru. Gra toczy się w czytelnym trybie tekstowym.

### Zasady sterowania:
1. **Format ruchu:** Wpisujemy pola w notacji algebraicznej, np. `e2e4`.
2. **Roszada:** Przesuwamy króla o dwa pola w bok (np. `e1g1` dla krótkiej roszady).
3. **Promocja:** W obecnej wersji każdy pionek promowany jest automatycznie na **Hetmana (Q)**.

### Legenda Symboli (Wielkie - Białe | małe - Czarne):
* **K / k** – Król (King)
* **Q / q** – Hetman (Queen)
* **R / r** – Wieża (Rook)
* **B / b** – Goniec (Bishop)
* **N / n** – Skoczek (Knight)
* **I / i** – Pionek (Pawn)

---

## 🛠️ Technologie i Standardy
* **Język:** C++ (Standard 17/20)
* **Optymalizacja:** Wykorzystanie instrukcji sprzętowych procesora (intrinsics: `_BitScanForward64`, `__popcnt64`) poprzez nagłówek `<intrin.h>`.
* **Standard pozycji:** Pełna obsługa notacji FEN (Forsyth-Edwards Notation).

## 🚀 Plan Rozwoju
- [ ] Implementacja protokołu **UCI** (Universal Chess Interface).
- [ ] Wdrożenie **Tablicy Transpozycji (TT)** dla jeszcze głębszego przeszukiwania.
- [ ] Dodanie **Quiescence Search** (szukanie spokoju), aby wyeliminować "efekt horyzontu" przy wymianach figur.

---

### Jak uruchomić?
1. Skompiluj projekt w środowisku Visual Studio (MSVC) lub za pomocą GCC/Clang.
2. Uruchom plik binarny.
3. Przeczytaj instrukcję wyświetloną na ekranie i naciśnij **ENTER**, aby rozpocząć partię!
