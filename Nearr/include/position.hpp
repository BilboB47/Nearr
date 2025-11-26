#pragma once
#include "move.hpp"
#include <cstdint>
#include <string>

class Position {
public:
    uint64_t bitBoard[14];
    bool isWhiteMove;
    uint8_t castlingRights;
    uint8_t enPassantSquare; //MOŻNA DODAĆ FUNKCJONALNOŚĆ enPassant
    uint16_t moveNumber;

public:
    void set_start_position();
    void set_position(std::string s,bool isWhite);
    void make_move(const Move& move);
    uint8_t piece_on_square(int sq) const;
    uint8_t piece_char_to_number(const char c);
};