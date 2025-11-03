#pragma once
#include "move.hpp"
#include <cstdint>

class Position {
public:
    uint64_t board[14];
    bool isWhiteMove;
    uint8_t castlingRights;
    uint8_t enPassantSquare;
    uint16_t moveNumber;

    void set_start_position();
    void make_move(const Move& move);
};