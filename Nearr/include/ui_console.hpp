#pragma once
#include "position.hpp"
#include "search.hpp"

#include <iostream>

using namespace std;



//===="konwersja================================================================
char get_piece_from_number(uint8_t n);
std::string squareToString(int index);
std::string moveToString(const Move m);

//=====wybor Gracza=========================================================
Color choose_color();
Move choose_move(const std::vector<Move>& legal_moves);

//=====napisy==========================================================================
void printVictory();
void printDefeat();
void printDraw();
void print_instructions();

//====Rozpoczecie gry==========================================================
void start_game();
