#include <iostream>
#include "ui_console.hpp"
#include "position.hpp"
#include "movegen.hpp"

#include <chrono>


void print_bitboard(uint64_t bitboard) {
    std::cout << "  ---------------------------------\n";
    // Plansza jest standardowo numerowana od a1 (0) do h8 (63).
    // Wypisujemy od rzędu 8 do rzędu 1.
    for (int rank = 7; rank >= 0; --rank) {
        std::cout << rank + 1 << " |"; // Numer rzędu (1-8)

        for (int file = 0; file < 8; ++file) {
            // Indeks pola: rank * 8 + file
            int square = rank * 8 + file;

            // Sprawdzenie, czy bit na danym polu jest ustawiony
            // Używamy operacji AND bitowego
            if (bitboard & (1ULL << square)) {
                std::cout << " X |"; // X - pole jest zajęte/atakowane
            }
            else {
                std::cout << " . |"; // . - pole jest wolne
            }
        }
        std::cout << "\n  ---------------------------------\n";
    }
    // Oznaczenia kolumn (A-H)
    std::cout << "    a   b   c   d   e   f   g   h\n";
}


int main() {
	//initKingAttacks();
	//initKnightAttacks();

	//Position pozycja = Position();
	//pozycja.set_position("r3qrk1/4bppp/p1N1pn2/1p6/5B2/2P5/PP3PPP/R2QR1K1",true);
	////pozycja.set_start_position();  

	//char board[8][8];
	//ToArray(pozycja, board);
	//printBoard(board);
	//cout << endl;

    /*initRookMagics();
    run_speed_comparison();*/

    
}
