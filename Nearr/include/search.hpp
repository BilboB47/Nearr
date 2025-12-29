#pragma once

#include "position.hpp"
#include "movegen.hpp"
#include "eval.hpp"

//================WARTOŒCI MATA ORAZ MAX VALUE===================================================
const int INF = 1000000;
const int MATE_VALUE = 900000;

struct TTEntry {
    uint64_t key;      // Zobrist Key
    int score;         // Ocena pozycji
    Move bestMove;     // Najlepszy ruch znaleziony w tej pozycji
    int16_t depth;     // G³êbokoœæ szukania
    TTFlag flag;      // Typ wyniku (Exact, Alpha, Beta)
};


const int TT_SIZE = 1024 * 1024 * 4;

class Search {
public:
    long long nodes;

    TTEntry* TT_table;
    Move killerMoves[64][2];

    uint64_t history_stack[1024]; 
    int history_size;         

    Search() : nodes(0) {

        TT_table = new TTEntry[TT_SIZE];
        if (TT_table) {
            memset(TT_table, 0, sizeof(TTEntry) * TT_SIZE);
        }

        history_size = 0;
        memset(killerMoves, 0, sizeof(killerMoves));
        memset(history_stack, 0, sizeof(history_stack));
    }

    ~Search() {
        delete[] TT_table;
    }

    Move get_best_move(Position& pos, int max_depth);
private:
    bool is_repetition(uint64_t current_key);
    int alpha_beta(Position& pos, int depth, int alpha, int beta, int root_depth, int ply);
    int quiescence(Position& pos, int alpha, int beta);
    int score_move(Position& pos, Move move, int depth);
};

