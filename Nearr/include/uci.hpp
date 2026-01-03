#ifndef UCI_HPP
#define UCI_HPP

#include "position.hpp"
#include "search.hpp"
#include "defs.hpp"
#include <string>


// Zamienia tekst typu "e2e4" na obiekt Move
Move parse_move_string(Position& pos, std::string move_str);

// G³ówna pêtla komunikacji z GUI/Lichess
void uci_loop();

#endif