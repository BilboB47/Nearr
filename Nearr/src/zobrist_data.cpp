#include "zobrist_data.hpp"

void init_zobrist_keys() {

	std::mt19937_64 rng(std::time(0));


	for (int i = 0; i < NUM_SQUARES; i++) {
		for (int j = 0; j < NUM_PIECE_TYPES; j++) {
			Zobrist.pieces[i][j] = rng();
		}	
	}

	for (int i = 0; i < NUM_CASTLING_STATES; i++) {
		Zobrist.castling[i] = rng();
	}

	for (int i = 0; i < NUM_FILES; i++) {
		Zobrist.enPassant[i] = rng();
	}

	Zobrist.sideToMove = rng();
}
