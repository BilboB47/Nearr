#include <iostream>
#include "ui_console.hpp"
#include "position.hpp"
#include "movegen.hpp"
#include "search.hpp"
#include "eval.hpp"

#include <chrono>
#include <iomanip>


bool compare_position(const Position& p1, const Position& p2) {
    for (int i = 0; i < 14; i++) {
        if (p1.bitBoard[i] != p2.bitBoard[i]) return false;
    }

    if (p1.isWhiteMove != p2.isWhiteMove) return false;
    if (p1.castlingRights != p2.castlingRights) return false;
    if (p1.enPassantSquare != p2.enPassantSquare) return false;
    if (p1.halfmoveClock != p2.halfmoveClock) return false;
    if (p1.moveNumber != p2.moveNumber) return false;
    if (p1.zobristKey != p2.zobristKey) return false;

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
bool test_compree(const Position& pos, const Position& copy2) {
    bool isCorrect = true;

    if (!compare_position(pos, copy2)) {
        isCorrect = false;

        // Sprawdzanie Bitboardów
        for (int b = 0; b < 14; b++) {
            if (pos.bitBoard[b] != copy2.bitBoard[b]) {
                std::cout << " -> Bitboard [" << b << "] się różni!" << std::endl;
                // Wypisywanie hex z zerami ułatwia lokalizację bitu (np. 0x40 to 0000000000000040)
                std::cout << "    Było: 0x" << std::hex << std::setw(16) << std::setfill('0') << copy2.bitBoard[b] << std::endl;
                std::cout << "    Jest: 0x" << std::hex << std::setw(16) << std::setfill('0') << pos.bitBoard[b] << std::dec << std::endl;
            }
        }

        // Sprawdzanie En Passant
        if (pos.enPassantSquare != copy2.enPassantSquare) {
            std::cout << " -> EnPassant się różni!" << std::endl;
            std::cout << "    Było: " << (int)copy2.enPassantSquare << " | Jest: " << (int)pos.enPassantSquare << std::endl;
        }

        // Sprawdzanie Zobrist
        if (pos.zobristKey != copy2.zobristKey) {
            std::cout << " -> ZobristKey się różni!" << std::endl;
            std::cout << "    Było: 0x" << std::hex << copy2.zobristKey << " | Jest: 0x" << pos.zobristKey << std::dec << std::endl;
        }

        // Sprawdzanie Roszad
        if (pos.castlingRights != copy2.castlingRights) {
            std::cout << " -> CastlingRights się różni!" << std::endl;
            std::cout << "    Było: " << (int)copy2.castlingRights << " | Jest: " << (int)pos.castlingRights << std::endl;
        }

        // Sprawdzanie Metadanych
        if (pos.isWhiteMove != copy2.isWhiteMove) {
            std::cout << " -> isWhiteMove się różni! (KRYTYCZNE)" << std::endl;
        }
        if (pos.moveNumber != copy2.moveNumber) {
            std::cout << " -> moveNumber się różni!" << std::endl;
        }

        std::cout << "--------------------------------------\n" << std::endl;
    }

    return isCorrect;
}



uint64_t perft_fast(Position& pos, int depth) {
    if (depth == 0) {
        return 1;
    }
    

    uint64_t nodes = 0;
    Move moves[256];
    int count = generateMoves(pos, moves);

    for (int i = 0; i < count; i++) {
        UndoInfo info = pos.make_move(moves[i]);

        // Sprawdzamy legalność (król nie może być bity)
        if (!is_in_check_enemy(pos)) {
            nodes += perft_fast(pos, depth - 1);
        }

        pos.unmake_move(moves[i], info);
    }
    return nodes;
}
void perft_divider(Position& pos, int depth) {
    uint64_t total_nodes = 0;
    Move moves[256];
    int count = generateMoves(pos, moves);

    std::cout << "--- Perft Divider Depth " << depth << " ---" << std::endl;

    for (int i = 0; i < count; i++) {
        UndoInfo info = pos.make_move(moves[i]);

        if (is_in_check_enemy(pos)) {
            pos.unmake_move(moves[i], info);
            continue;
        }

        // Tutaj obliczamy pod-węzły dla tego konkretnego ruchu
        uint64_t nodes = perft_fast(pos, depth - 1);
        pos.unmake_move(moves[i], info);

        total_nodes += nodes;
    }

    std::cout << "=================================" << std::endl;
    std::cout << "Suma wszystkich wezlow: " << total_nodes << std::endl;
}

void run_benchmark(Position& pos, int depth) {
    std::cout << "Rozpoczynam test wydajnosci (Perft depth " << depth << ")..." << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    uint64_t nodes = perft_fast(pos, depth);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    double nps = nodes / elapsed.count();

    std::cout << "---------------------------------" << std::endl;
    std::cout << "Wynik: " << nodes << " wezlow" << std::endl;
    std::cout << "Czas:  " << elapsed.count() << " s" << std::endl;
    std::cout << "NPS:   " << nps / 1000000.0 << " MNPS (milionow wezlow/s)" << std::endl;
    std::cout << "---------------------------------" << std::endl;
}


int main() {

    initAttackTables();
    init_zobrist_keys();
    init_pst();

    Position pos;
    
    //preft pozycje
    pos.set_position_FEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");//git 3
    //pos.set_position_FEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");//git ale bylo źle
    //pos.set_position_FEN("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");//git 3
    //pos.set_position_FEN("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"); //git 3
    //pos.set_position_FEN("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 "); //git 3
    //pos.set_position_FEN("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");//blad d=3
    //pos.set_position_FEN("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10"); //git
    //pos.set_position_FEN("4k2K/8/8/8/8/8/8/8 w - - 0 1"); //git
     
    //ewaluja   
    //pos.set_position_FEN("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1"); //git
    //pos.set_position_FEN("8/4P3/8/8/8/8/4k3/4K3 w - - 0 1"); //git
    //pos.set_position_FEN("4k3/8/8/3N4/8/8/8/4K3 w - - 0 1"); //git
    //pos.set_position_FEN("N3k3/8/8/8/8/8/8/4K3 w - - 0 1"); //git
    //pos.set_position_FEN("4k3/8/8/8/8/8/PPPPPPPP/4K3 w - - 0 1"); //git
    //pos.set_position_FEN("r1b1kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKB1R w KQkq - 0 1"); //git
    
    pos.set_position_FEN("r3k2r/p2bbpp1/2p4p/8/1ppPP3/P1N1B2P/1PP2PP1/R4RK1 w kq - 0 1"); //git

    pos.print_board();
    //cout << evaluate_all(pos);
    //run_benchmark(pos,5);


    Search engine;

    int depth = 12;

    std::cout << "Silnik mysli..." << std::endl;

    // Wywołujemy szukanie
    Move bestMove = engine.get_best_move(pos, depth);

    pos.print_board();

    return 0;

}





