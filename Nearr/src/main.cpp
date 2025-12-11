#include <iostream>
#include "ui_console.hpp"
#include "position.hpp"
#include "movegen.hpp"

#include <chrono>
#include <iomanip>

using namespace std::chrono;

void benchmark(Position& pos)
{
    const int iterations = 3;

    auto start = std::chrono::high_resolution_clock::now();

    volatile int sink = 0;

    for (int i = 0; i < iterations; i++)
    {
        Move moves[256];
        int count = generateMoves(pos, moves);

        sink += count;   // zapobiega optymalizacji
    }

    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    double avg_ns = double(duration) / iterations;

    std::cout << "Średni czas jednego generateMoves: "
        << avg_ns << " ns\n";
}

std::string square_to_string(int sq) {
    char file = 'a' + (sq % 8);
    char rank = '1' + (sq / 8);
    return std::string() + file + rank;
}



int main() {

    initAttackTables();

    Position pos;


    pos.set_position_FEN("1k6/6Pp/8/8/8/8/8/K7 w - - 0 1");
    pos.print_board();

    Move moves[256];

    
    int count = generateMoves(pos, moves);
    pos.make_move(moves[0]);
    //cout << count;
    pos.print_board();

   


    //benchmark(pos);
   


    return 0;

}
