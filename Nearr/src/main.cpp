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
#include "bitboard_utils.hpp"
#include <bitset>

uint64_t perft(Position& pos, int depth) {
    if (depth == 0) return 1;

    uint64_t nodes = 0;
    Move moves[256];
    int count = generateMoves(pos, moves);

    for (int i = 0; i < count; i++) {
        // 1. Zapisujemy stan IDEALNY przed ruchem
        Position copy_before = pos;

        // 2. Wykonujemy ruch
        UndoInfo info = pos.make_move(moves[i]);

        // 3. Sprawdzamy legalność
        if (is_in_check_enemy(pos)) {
            pos.unmake_move(moves[i], info);

            // TEST: Czy unmake zadziałał poprawnie dla ruchu nielegalnego?
            if (!test_compree(pos, copy_before)) {
                std::cout << "Stan planszy po blednym unmake:" << std::endl;
                pos.print_board(); // Wypisuje aktualna (bledna) plansze

                std::cout << "\nNacisnij ENTER, aby kontynuowac debugowanie..." << std::endl;
                std::cin.get();
            }
            continue;
        }

        // 4. Rekurencja
        nodes += perft(pos, depth - 1);

        // 5. Cofamy ruch
        pos.unmake_move(moves[i], info);

        // 6. TEST: Sprawdzamy, czy po unmake plansza wróciła do stanu copy_before
        if (!test_compree(pos, copy_before)) {
            std::cout << "Glebokosc (depth): " << depth << std::endl;
            std::cout << "Stan planszy po blednym unmake:" << std::endl;
            pos.print_board(); // Wypisuje aktualna (bledna) plansze

            std::cout << "\nNacisnij ENTER, aby kontynuowac debugowanie..." << std::endl;
            std::cin.get();
        }
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
        uint64_t nodes = perft(pos, depth - 1);
        pos.unmake_move(moves[i], info);

        total_nodes += nodes;
    }

    std::cout << "=================================" << std::endl;
    std::cout << "Suma wszystkich wezlow: " << total_nodes << std::endl;
}

void print_castling_rights(uint8_t rights) {
    std::cout << "Prawa do roszady: ";
    if (rights == 0) {
        std::cout << "-";
    }
    else {
        if (rights & WK) std::cout << "K";
        if (rights & WQ) std::cout << "Q";
        if (rights & BK) std::cout << "k";
        if (rights & BQ) std::cout << "q";
    }
    // Opcjonalnie wypisz wartość binarną dla debugowania
    std::cout << " (bitowo: " << (int)rights << ")" << std::endl;
}

int main() {

    initAttackTables();
    init_zobrist_keys();





    Position pos;
    //pos.set_position_FEN("r5k1/pP6/8/8/8/8/8/4K3 b - - 0 1");
    //pos.set_position_FEN("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/4K3 b - - 0 1");
    
    //pos.set_position_FEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");//git 3
    //pos.set_position_FEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");//git ale bylo źle
    //pos.set_position_FEN("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");//git 3
    //pos.set_position_FEN("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"); //git 3
    //pos.set_position_FEN("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 "); //git 3
    //pos.set_position_FEN("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");//blad d=3
    pos.set_position_FEN("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10"); //git

    perft_divider(pos, 4);

    return 0;

    //pos.set_position_FEN("rnbqkbnr/pppppppp/8/8/8/8/8/R3K2R w KQkq - 0 1");//blad d=3
    
    //pos.set_position_FEN("4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1");//blad d=3


    //Position c = pos;
    //pos.print_board();
    //print_castling_rights(pos.castlingRights);
    //Move m[256];
    //generateMoves(pos, m); //23           22
    //UndoInfo i = pos.make_move(m[20]);
    //pos.print_board();
    //print_castling_rights(pos.castlingRights);

    //pos.unmake_move(m[0], i);

    //pos.print_board();
    
    //if (!test_compree(pos, c)) {
    //    cout << "d";
    //}


   //return 0;

    pos.print_board();



    int legalMovesCount = 0;  

    Move moves[256];
    int count = generateMoves(pos, moves);

    for (int i = 0; i < count; i++) {
        // 1. Zapisujemy stan przed ruchem
        Position copy1 = pos;

        // 2. Wykonujemy ruch na oryginalnym obiekcie 'pos'
        pos.make_move(moves[i]);

        // 3. Sprawdzamy czy król nie jest pod atakiem
        if (is_in_check_enemy(pos)) {
            pos = copy1; // BŁĄD: ruch nielegalny, przywracamy stan z kopii
            continue;
        }

            pos.print_board();

        // --- POZIOM 2 ---
        Move moves2[256];
        int count2 = generateMoves(pos, moves2);
        for (int j = 0; j < count2; j++) {
            Position copy2 = pos; // Kopia przed drugim ruchem

            pos.make_move(moves2[j]);

            if (is_in_check_enemy(pos)) {
                pos = copy2; // Przywracamy stan sprzed moves2[j]
                continue;
            }
            pos.print_board();
       
            // --- POZIOM 3 ---
            Move moves3[256];
            int count3 = generateMoves(pos, moves3);
            for (int k = 0; k < count3; k++) {
                Position copy3 = pos; // Kopia idealna
                UndoInfo info3 = pos.make_move(moves3[k]);

                // Testujemy unmake
                pos.unmake_move(moves3[k], info3);

                if (!test_compree(pos, copy3)) {
                    std::cout << "\n==========================================" << std::endl;
                    std::cout << "ALARM: unmake_move nie przywrocilo stanu!" << std::endl;
                    std::cout << "Indeks ruchu w liscie: " << k << std::endl;

                    // Dane o ruchu
                    int from = moves3[k].from;
                    int to = moves3[k].to;
                    int flags = moves3[k].flags;

                    std::cout << "Ruch: " << square_to_string(from) << " -> " << square_to_string(to) << std::endl;
                    std::cout << "Flaga (binarnie): " << std::bitset<4>(flags) << std::endl;

                    // Szczegoly o zbitej figurze
                    if (info3.capturePiece != NO_PIECE) {
                        std::cout << "Zbita figura (ID): " << (int)info3.capturePiece << std::endl;
                    }

                    // Interpretacja typu ruchu
                    std::string typ = "Zwykly";
                    if (flags == FLAG_CASTLE_KINGSIDE) typ = "Roszada krotka";
                    else if (flags == FLAG_CASTLE_QUEENSIDE) typ = "Roszada dluga";
                    else if (flags == FLAG_EN_PASSANT) typ = "Bicie w przelocie";
                    else if (flags & 0b1000) typ = "Promocja";

                    std::cout << "Typ ruchu: " << typ << std::endl;
                    std::cout << "==========================================\n" << std::endl;

                    // Wypisz roznice bitboardow (test_compree to robi automatycznie u Ciebie)
                }

                // Zawsze przywracamy kopie, zeby bledne unmake nie popsulo reszty testu Perft
                pos = copy3;
            }
  


            // Przywracamy stan po zakończeniu wszystkich ruchów moves3
            pos = copy2;
        }

        // Przywracamy stan po zakończeniu poziomu 2
        pos = copy1;
    }



    cout << "Legal moves (Depth 1): " << legalMovesCount << endl;




    return 0;

}
