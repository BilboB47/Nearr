#pragma once
#include "move.hpp"
#include <cstdint>

class Position {
public:
    uint64_t bitBoard[14];
    bool isWhiteMove;
    uint8_t castlingRights;
    uint8_t enPassantSquare; //MOŻNA DODAĆ FUNKCJONALNOŚĆ enPassant
    uint16_t moveNumber;

public:
    void set_start_position();
    void make_move(const Move& move);
    uint8_t piece_on_square(int sq) const;
};