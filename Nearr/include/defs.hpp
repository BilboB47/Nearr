#pragma once
#include <cstdint>

enum Piece : uint8_t { //s¹ równoznacze z indexami w bitboard
    WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING,
    BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING,
    WHITE_ALL, BLACK_ALL,
    NO_PIECE = 255
};


enum Square : uint8_t {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    NO_SQUARE = 255
};

/// 8 | 56 57 58 59 60 61 62 63
/// 7 | 48 49 50 51 52 53 54 55
/// 6 | 40 41 42 43 44 45 46 47
/// 5 | 32 33 34 35 36 37 38 39
/// 4 | 24 25 26 27 28 29 30 31
/// 3 | 16 17 18 19 20 21 22 23
/// 2 | 8  9 10 11 12 13 14 15
/// 1 | 0  1  2  3  4  5  6  7
///     A  B  C  D  E  F  G  H
    

enum Color { WHITE, BLACK };

enum Flags : uint8_t {
    FLAG_NONE = 0,

    FLAG_PAWN_DOUBLE_PUSH = 1 << 0,
    FLAG_EN_PASSANT = 1 << 1,
    
    FLAG_CASTLE_KINGSIDE = 1 << 2,
    FLAG_CASTLE_QUEENSIDE = 1 << 3,

    FLAG_PROMOTION = 1 << 4,
};

enum Promotion : uint8_t {
    PROMOTION_NONE = 0,

    PROMOTION_BISHOP = 1 << 0,          
    PROMOTION_KNIGHT = 1 << 1,
    PROMOTION_ROOK = 1 << 2,
    PROMOTION_QUEEN = 1 << 3,
};

enum CastlingRights : uint8_t {
    WK = 1,  // 0001: White Kingside (Krótka Bia³e)
    WQ = 2,  // 0010: White Queenside (D³uga Bia³e)
    BK = 4,  // 0100: Black Kingside (Krótka Czarne)
    BQ = 8,  // 1000: Black Queenside (D³uga Czarne)
};






