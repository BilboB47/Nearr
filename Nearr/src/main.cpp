#include <iostream>
#include "ui_console.hpp"
#include "position.hpp"

int main() {
	Position p = Position();
	p.set_start_position();
	char board[8][8];

	Move ruch(F1,F4,WHITE_BISHOP);
	p.make_move(ruch);

	

	ToArray(p,board);
	printBoard(board);
}
