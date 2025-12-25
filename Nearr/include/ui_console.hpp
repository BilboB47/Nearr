#pragma once
#include <iostream>
#include "position.hpp"
#include "movegen.hpp"
#include "search.hpp"


using namespace std;

char get_piece_from_number(uint8_t n);
std::string squareToString(int index);
std::string moveToString(const Move m);
void game();