# Nearr - Silnik Szachowy C++

Kompaktowy, silnik szachowy, zaimplementowany w C++ z wykorzystaniem techniki Bitboards (tablice bitowe) dla optymalnej wydajności w manipulacji stanem planszy.

---

## 🎯 Status Projektu

Projekt znajduje się w fazie **implementacji mechanik podstawowych**. Pełna obsługa ruchów (w tym Roszada, En Passant, Promocja) jest już zaimplementowana. Aktualnie trwają prace nad **Generatorem Ruchów Pseudo-Legalnych** i funkcją weryfikacji szacha.

## ⚙️ Kluczowe Technologie i Struktury Danych

* **Język:** C++
* **Architektura Planszy:** Bitboards (64-bitowe liczby całkowite)

## 🛠️ Zaimplementowane Mechaniki

Poniższe mechaniki są w pełni zaimplementowane, przetestowane i zarządzane w ramach funkcji `Position::make_move()`.

### I. Inicjalizacja i Stan Planszy

* **Pełne Parsowanie FEN:** Poprawne wczytywanie pozycji z notacji FEN (łącznie z turą, prawami roszady i polem En Passant).
* **Zarządzanie Stanem:** Struktura `Position` przechowuje Bitboards, prawa roszady (`castlingRights`) oraz zmienną `isWhiteMove` (tura gracza).
* **Wizualizacja:** Funkcja `print_board()` do debugowania.

### II. Wykonywanie Ruchów (`make_move`)

Logika wykonywania ruchów jest modularna i rozbita na mniejsze, testowalne funkcje pomocnicze.

* **Ruchy Podstawowe:**
    * `make_simple_move(piece, from, to)`
    * `remove_captured_piece(captured, to)`
* **Obsługa Ruchów Specjalnych:**
    * **Roszada (Castling):** Logika przenoszenia Króla i Wieży (`handle_castling_rook`).
    * **Promocja (Promotion):** Zaimplementowana funkcja `promote_pawn(move, to_bb)`.
    * **Bicie En Passant:** Dedykowana obsługa bicia pionka na polu pośrednim.
* **Zarządzanie Prawami:** W pełni zaimplementowana i przetestowana funkcja `update_castling_rights()`, która poprawnie usuwa prawa roszady po ruchu Króla, Wieży lub zbiciu Wieży przeciwnika.

### III. Generator Ruchów (W Trakcie)

* **Tablice Ataku:** Statycznie wygenerowane tablice dla ruchów pionków (`pawnAttacks`, `pawnMoves`) oraz ataki Króla (`kingAttacks`) dla szybkiego dostępu do potencjalnych pól docelowych.
* **Szkielet Generatorów:** Zainicjowano funkcje `generatePawnMoves()` i `generateKingMoves()` do generowania ruchów pseudo-legalnych.

## 💻 Struktura Ruchu (`Move`)

Ruch jest skompresowany do 32-bitowej liczby całkowitej (`uint32_t`) za pomocą pól bitowych (bit fields) dla optymalizacji pamięci.

```cpp
struct Move
{
	uint32_t from : 6;       // 0-63
	uint32_t to : 6;         // 0-63
	uint32_t piece : 4;      // Typ poruszanej figury
	uint32_t captured : 4;   // Typ zbitej figury
	uint32_t flags : 5;      // Flagi specjalne (EP, Roszada, Promocja, Podwójne pchnięcie)
	uint32_t promotion : 4;  // Typ figury promowanej (Q, R, B, N)
};
