#pragma once
#include "move.hpp"
#include <iostream>
#include <cstdint>
#include <string>

class Position {
public:
    uint64_t bitBoard[14];
    bool isWhiteMove;
    uint8_t castlingRights; // 0001-W_K |0010- W_Q | 0100-B_K | 1000-B_Q
    uint8_t enPassantSquare; 
    uint16_t moveNumber;
    uint8_t halfmoveClock;

    uint64_t zobristKey;

public:
    //===============================USTAWIANIE POZYCJI==================================================
    uint8_t piece_char_to_number(const char c);
    void set_start_position();
    void set_position_FEN(std::string s);

    //========================================ROBIENIE RUCHU===============================================================
    void make_simple_move(uint8_t piece_type, uint64_t from_bb, uint64_t to_bb, Piece moving_color_all);
    void remove_captured_piece(uint8_t captured_piece, uint64_t captured_square_bb, Piece captured_color_all);
    void promote_pawn(const Move& move, uint64_t to_bb);
    void update_castling_rights(const Move& move);
    void handle_castling_rook(const Move& move);

    void make_move(const Move& move);

    //===================ZBIERANIE INFORMACJI NA TEMAT PLANSZY==============================================================
    uint64_t getAllPieces() const;
    uint64_t getAllFriendlyPieces() const;
    uint64_t getAllEnemyPieces() const;
    uint8_t piece_on_square(int sq) const;

    char piece_to_char(int piece_index);
    void print_board();
};