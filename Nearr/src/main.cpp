#include <iostream>
#include "ui_console.hpp"
#include "position.hpp"
#include "movegen.hpp"

#include <chrono>
#include <iomanip>
#include "main.h"

using namespace std::chrono;

std::string square_to_string(int sq) {
    char file = 'a' + (sq % 8);
    char rank = '1' + (sq / 8);
    return std::string() + file + rank;
}

void benchmark(Position& pos)
{
    const int iterations = 100000;

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

bool compare_position(const Position& p1, const Position& p2) {
    for (int i = 0; i < 14; i++) {
        if (p1.bitBoard[i] != p2.bitBoard[i]) return false;
    }

    if (p1.isWhiteMove != p2.isWhiteMove) return false;
    if (p1.castlingRights != p2.castlingRights) return false;
    if (p1.enPassantSquare != p2.enPassantSquare) return false;

    if (p1.halfmoveClock != p2.halfmoveClock) return false;
    if (p1.moveNumber != p2.moveNumber) return false;

    if (p1.zobristKey != p2.zobristKey) {
        return false;
    }

    return true;
}

void test_unmake() {
    Position pos;
    pos.set_position_FEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

    pos.print_board();
    cout << endl << pos.zobristKey << endl;

    Move moves[256];
    int count = generateMoves(pos, moves);
    UndoInfo info = pos.make_move(moves[1]);

    pos.print_board();
    cout << endl << pos.zobristKey << endl;

    pos.unmake_move(moves[1], info);
    pos.print_board();
    cout << endl << pos.zobristKey << endl;

    Position posC;
    posC.set_position_FEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

    cout << compare_position(pos, posC);
}

void show_moves()
{
    Position pos;
    pos.set_position_FEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    pos.print_board();

    Move moves[256];
    int count = generateMoves(pos, moves);
    for (int i = 0; i < count; i++) {
        cout << "===================pozycja " << i + 1 << " ===========================" << endl;

        UndoInfo info = pos.make_move(moves[i]);

        pos.print_board();
        cout << endl << pos.zobristKey << endl;

        pos.unmake_move(moves[i], info);
    }
}

#include "bitboard_utils.hpp"

int main() {

    initAttackTables();
    init_zobrist_keys();

    Position pos;
    pos.set_position_FEN("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1 ");


    Move moves[256];
    int count = generateMoves(pos, moves);

    int legalMovesCount2 = 0;   // Perft(1)

    for (int i = 0; i < count; i++) {
        UndoInfo info = pos.make_move(moves[i]);

        if (is_in_check_enemy(pos)) {
            pos.unmake_move(moves[i], info);
            continue;
        }
        cout << i << endl;
        pos.print_board();


        Move moves2[256];
        int count2 = generateMoves(pos, moves2);

        for (int i = 0; i < count2; i++) {
            UndoInfo info = pos.make_move(moves2[i]);

            if (is_in_check_enemy(pos)) {
                pos.unmake_move(moves2[i], info);
                continue;
            }
            cout << i << endl;
            pos.print_board();

            legalMovesCount2++; // Ten ruch jest legalny!

            pos.unmake_move(moves2[i], info); // Cofamy ruch z Depth 1
        }


        pos.unmake_move(moves[i], info); // Cofamy ruch z Depth 1
    }





    cout << "Legal moves (Depth 1): " << legalMovesCount2 << endl;




    return 0;

}
