#pragma once

#include "position.hpp"
#include "movegen.hpp"
#include "eval.hpp"

const int INF = 1000000;
const int MATE_VALUE = 900000;

class Search {
public:
    long long nodes; //przydatne do bnech mark

    Move get_best_move(Position& pos, int depth);

private:
    int alpha_beta(Position& pos, int depth, int alpha, int beta);

};

