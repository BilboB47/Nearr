#pragma once
#include "defs.hpp"
#include "move.hpp"
#include "position.hpp"
#include <vector>
#include <intrin.h>
#include <random>


//====================================FUNKCJE BITOWE=============================================================
inline int pop_lsb(uint64_t* bitboard);
inline int count_set_bits(uint64_t bb);

//====================================GENEROWANIE LICZB LOSOWYCH=============================================================
uint64_t random_uint64(std::mt19937_64& generator, std::uniform_int_distribution<uint64_t>& dystrybucja);
uint64_t generate_filtered_magic_candidate(std::mt19937_64& generator, std::uniform_int_distribution<uint64_t>& dystrybucja);

//====================================GENEROWANIE WARIANTÓW=============================================================
void generate_blocker_patterns(uint64_t mask, int num_bits, std::vector<uint64_t>& patterns);

//====================================WIE¯A=========================================================================
extern uint64_t RookMasks[64];
extern uint64_t RookMagicNumbers[64];
extern uint64_t RookShifts[64];
extern uint64_t RookOffsets[64];
const size_t ROOK_ATTACK_TABLE_SIZE = 102400;
extern uint64_t RookAttackTable[ROOK_ATTACK_TABLE_SIZE];


void initRookMasks();
uint64_t get_rook_attacks_slow(int square, uint64_t blockers);
void initRookMagics();
uint64_t get_rook_attacks(int square, uint64_t board);
void generateRookMoves(const Position& pos, std::vector<Move>& moves);


//====================================GONIEC=========================================================================
extern uint64_t BishopMasks[64];
extern uint64_t BishopMagicNumbers[64];
extern uint64_t BishopShifts[64];
extern uint64_t BishopOffsets[64];
const size_t BISHOP_ATTACK_TABLE_SIZE = 23040;
extern uint64_t BishopAttackTable[BISHOP_ATTACK_TABLE_SIZE];

void initBishopMasks();
uint64_t get_bishop_attacks_slow(int square, uint64_t blockers);
void initBishopMagics();
uint64_t get_bishop_attacks(int square, uint64_t board);
void generateBishopMoves(const Position& pos, std::vector<Move>& moves);


//====================================SKOCZEK=============================================================
extern uint64_t knightAttacks[64];
void initKnightAttacks();
void generateKnightMoves(const Position& pos, std::vector<Move>& moves);

//====================================KRÓL=============================================================
extern uint64_t kingAttacks[64];
void initKingAttacks();
void generateKingMoves(const Position& pos, std::vector<Move>& moves);


//====================================PION=============================================================
void generatePawnMoves(const Position& pos, std::vector<Move>& moves);




//====================================HETMAN=============================================================
void generateQueenMoves(const Position& pos, std::vector<Move>& moves);


void initAttackTables();
std::vector<Move> generateMoves(const Position& pos);








