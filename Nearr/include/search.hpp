#pragma once

#include "position.hpp"
#include "movegen.hpp"
#include "eval.hpp"

const int INF = 1000000;
const int MATE_VALUE = 900000;


class Search {
public:
    long long nodes; //przydatne do bnech mark
    uint64_t history_stack[1024]; 
    int history_size = 0;

    Move get_best_move(Position& pos, int depth);

private:
    bool is_repetition(Position& pos);
    int alpha_beta(Position& pos, int depth, int alpha, int beta, int root_depth);
    int score_move(Position& pos, Move move);
    //int quiescence(Position& pos, int alpha, int beta);
};

