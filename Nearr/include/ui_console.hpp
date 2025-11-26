#pragma once
#include <iostream>
#include "position.hpp"
#include "movegen.hpp"

using namespace std;


void ToArray(const Position &curP, char (&board)[8][8]);
wchar_t pieceToUnicode(char p);
void printBoard(char(&board)[8][8]);
void printBoard(wchar_t(&board)[8][8]);
std::string squareToString(int index);
void printMoves(std::vector<Move> &moves);