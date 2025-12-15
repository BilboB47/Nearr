#pragma once

#include <cstdint>
#include <random>

#define NUM_SQUARES 64  
#define NUM_PIECE_TYPES 12  
#define NUM_CASTLING_STATES 16 
#define NUM_FILES 8


struct ZobristTable {
    uint64_t pieces[NUM_SQUARES][NUM_PIECE_TYPES]; 
    uint64_t castling[NUM_CASTLING_STATES];
    uint64_t enPassant[NUM_FILES];
    uint64_t sideToMove;
};

extern ZobristTable Zobrist;

void init_zobrist_keys();