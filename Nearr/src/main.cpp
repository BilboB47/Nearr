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

// Funkcja pomocnicza do zapobiegania optymalizacji kompilatora
void consume_result(uint64_t result) {
    volatile uint64_t dummy = result;
}

// Implementacja iteracyjna (O(L)) - dla porównania prędkości
uint64_t get_rook_attacks_iterative(int square, uint64_t blockers) {
    uint64_t moves = 0ULL;
    int row = square / 8;
    int col = square % 8;

    for (int j = col + 1; j < 8; j++) {
        int target_square = row * 8 + j;
        moves |= (1ULL << target_square);
        if (blockers & (1ULL << target_square)) break;
    }
    for (int j = col - 1; j >= 0; j--) {
        int target_square = row * 8 + j;
        moves |= (1ULL << target_square);
        if (blockers & (1ULL << target_square)) break;
    }
    for (int j = row + 1; j < 8; j++) {
        int target_square = j * 8 + col;
        moves |= (1ULL << target_square);
        if (blockers & (1ULL << target_square)) break;
    }
    for (int j = row - 1; j >= 0; j--) {
        int target_square = j * 8 + col;
        moves |= (1ULL << target_square);
        if (blockers & (1ULL << target_square)) break;
    }
    return moves;
}


void run_speed_comparison() {
    // ⚠️ MUSISZ MIEĆ TĘ FUNKCJĘ WYWOŁANĄ RAZ PRZED TESTEM!
    // initRookMagics(); 

    const int ITERATIONS = 1000000000; // 1000 milionów prób
    int square = 41; // b6
    uint64_t board_blokerow = (1ULL << 42) | (1ULL << 57) | (1ULL << 17); // c6, b8, b3

    // -----------------------------------------------------
    // CZĘŚĆ 1: MBS (Magic Bitboards)
    // -----------------------------------------------------

    auto start_mbs = std::chrono::high_resolution_clock::now();

    // Dodano consume_result()
    for (int i = 0; i < ITERATIONS; ++i) {
        uint64_t attacks = get_rook_attacks(square, board_blokerow);
        consume_result(attacks); // ZAPOBIEGA OPTYMALIZACJI
    }

    auto end_mbs = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::nano> duration_mbs_ns = end_mbs - start_mbs;
    double time_per_op_mbs_ns = duration_mbs_ns.count() / ITERATIONS;
    double total_time_mbs_ms = duration_mbs_ns.count() / 1e6;

    // -----------------------------------------------------
    // CZĘŚĆ 2: ITERACYJNA
    // -----------------------------------------------------

    auto start_iter = std::chrono::high_resolution_clock::now();

    // Dodano consume_result()
    for (int i = 0; i < ITERATIONS; ++i) {
        uint64_t attacks = get_rook_attacks_iterative(square, board_blokerow);
        consume_result(attacks); // ZAPOBIEGA OPTYMALIZACJI
    }

    auto end_iter = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::nano> duration_iter_ns = end_iter - start_iter;
    double time_per_op_iter_ns = duration_iter_ns.count() / ITERATIONS;
    double total_time_iter_ms = duration_iter_ns.count() / 1e6;

    // -----------------------------------------------------
    // WYNIKI PORÓWNAWCZE
    // -----------------------------------------------------
    std::cout << "\n==================================================\n";
    std::cout << "          WYNIKI PORÓWNAWCZE PRĘDKOŚCI\n";
    std::cout << "==================================================\n";
    std::cout << "Liczba operacji: " << ITERATIONS << " (10 milionów)\n";
    std::cout << "Dane: Wieza na b6, Blokery: c6, b8, b3\n";
    std::cout << "--------------------------------------------------\n";

    // Wyniki MBS
    std::cout << "⭐ MBS (get_rook_attacks) - Szybka Metoda (O(1)):\n";
    std::cout << "   Całkowity czas: " << total_time_mbs_ms << " ms\n";
    std::cout << "   Czas na operację: **" << time_per_op_mbs_ns << " ns**\n";
    std::cout << "--------------------------------------------------\n";

    // Wyniki Iteracyjne
    std::cout << "🐢 Iteracyjna (get_rook_attacks_iterative) - Wolna Metoda (O(L)):\n";
    std::cout << "   Całkowity czas: " << total_time_iter_ms << " ms\n";
    std::cout << "   Czas na operację: **" << time_per_op_iter_ns << " ns**\n"; // ⬅️ POPRAWIONE!
    std::cout << "--------------------------------------------------\n";

    double speedup = time_per_op_iter_ns / time_per_op_mbs_ns;
    std::cout << "Współczynnik przyspieszenia MBS: ~" << speedup << "x\n";
    std::cout << "==================================================\n";
}

int main() {
	//initKnightAttacks();
	//initKingAttacks();

	//Position pozycja = Position();
	//pozycja.set_position("r3qrk1/4bppp/p1N1pn2/1p6/5B2/2P5/PP3PPP/R2QR1K1",true);
	////pozycja.set_start_position();

	//char board[8][8];
	//ToArray(pozycja, board);
	//printBoard(board);
	//cout << endl;

    initRookMagics();
    run_speed_comparison();

    cout << sizeof(RookAttackTable);
}
