#pragma once
#include "defs.hpp"
#include "move.hpp"
#include "position.hpp"
#include <array>
#include <vector>
#include <intrin.h>
#include <random>

extern uint64_t knightAttacks[64];
extern uint64_t kingAttacks[64];
//wie¿a
extern uint64_t RookMasks[64];
extern uint64_t RookMagicNumbers[64];
extern uint64_t RookShifts[64];
extern uint64_t RookOffsets[64];
const size_t ROOK_ATTACK_TABLE_SIZE = 102400;
extern uint64_t RookAttackTable[ROOK_ATTACK_TABLE_SIZE];


void initAttackTables();
void initKingAttacks();
void initKnightAttacks();

inline int pop_lsb(uint64_t* bitboard);
inline int count_set_bits(uint64_t bb);

void initRookMasks();
uint64_t get_rook_attacks_slow(int square, uint64_t blockers);
void generate_blocker_patterns(uint64_t mask, int num_bits, std::vector<uint64_t>& patterns);
uint64_t random_uint64(std::mt19937_64& generator, std::uniform_int_distribution<uint64_t>& dystrybucja);
uint64_t generate_filtered_magic_candidate(std::mt19937_64& generator, std::uniform_int_distribution<uint64_t>& dystrybucja);
void initRookMagics();
uint64_t get_rook_attacks(int square, uint64_t board);

std::vector<Move> generateMoves(const Position& pos);
void generatePawnMoves(const Position& pos, std::vector<Move>& moves);
void generateKnightMoves(const Position& pos, std::vector<Move>& moves);
void generateBishopMoves(const Position& pos, std::vector<Move>& moves);
void generateRookMoves(const Position& pos, std::vector<Move>& moves);
void generateQueenMoves(const Position& pos, std::vector<Move>& moves);
void generateKingMoves(const Position& pos, std::vector<Move>& moves);








