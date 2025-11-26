#pragma once
#include "defs.hpp"
#include "move.hpp"
#include "position.hpp"
#include <array>
#include <vector>

extern std::array<std::vector<int>, 64> knightAttacks;
extern std::array<std::vector<int>, 64> kingAttacks;

void initAttackTables();
void initKingAttacks();
void initKnightAttacks();
std::vector<Move> generateMoves(const Position& pos);
void generatePawnMoves(const Position& pos, std::vector<Move>& moves);
void generateKnightMoves(const Position& pos, std::vector<Move>& moves);
void generateBishopMoves(const Position& pos, std::vector<Move>& moves);
void generateRookMoves(const Position& pos, std::vector<Move>& moves);
void generateQueenMoves(const Position& pos, std::vector<Move>& moves);
void generateKingMoves(const Position& pos, std::vector<Move>& moves);








