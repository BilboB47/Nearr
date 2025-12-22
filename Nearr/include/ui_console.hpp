#pragma once
#include <iostream>
#include "position.hpp"
#include "movegen.hpp"

using namespace std;

char get_piece_from_number(uint8_t n);
std::string squareToString(int index);
void print_move(const Move m);