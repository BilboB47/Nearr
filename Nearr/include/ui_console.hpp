#pragma once
#include "position.hpp"
#include "search.hpp"

#include <iostream>
#include <chrono> // Pami�taj o dodaniu tego na g�rze pliku
#include <iomanip> // Przydatne do zaokr�glania NPS

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
void botVsBot();