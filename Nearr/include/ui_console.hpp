#pragma once
#include <iostream>
#include "position.hpp"

using namespace std;


void ToArray(const Position &curP, char (&board)[8][8]);
wchar_t pieceToUnicode(char p);
void printBoard(char(&board)[8][8]);
void printBoard(wchar_t(&board)[8][8]);