#include <iostream>
#include "ui_console.hpp"
#include "position.hpp"
#include "movegen.hpp"

#include <chrono>

int main() {
	initKnightAttacks();
	initKingAttacks();

	Position pozycja = Position();
	pozycja.set_position("r3qrk1/4bppp/p1N1pn2/1p6/5B2/2P5/PP3PPP/R2QR1K1",true);
	//pozycja.set_start_position();

	char board[8][8];
	ToArray(pozycja, board);
	printBoard(board);
	cout << endl;

    const int NUM_ITERATIONS = 100000; // Ilość powtórzeń testu

    // Zapewnienie, że kod działa w trybie Release i optymalizacje są włączone!

    // START: Pomiar całego bloku
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        generateMoves(pozycja);
    }

    // STOP: Pomiar całego bloku
    auto stop = std::chrono::high_resolution_clock::now();

    // Obliczenie całkowitego czasu
    auto total_duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);

    // Obliczenie czasu na jedną operację
    long long avg_duration_ns = total_duration_ns.count() / NUM_ITERATIONS;

    std::cout << "Całkowity czas dla " << NUM_ITERATIONS << " iteracji: "
        << total_duration_ns.count() << " ns." << std::endl;
    std::cout << "Średni czas na jedno wykonanie: "
        << avg_duration_ns << " ns." << std::endl;


	//cout << "liczba ruchow: " << ruchy.size() << endl;
	//printMoves(ruchy);
}
