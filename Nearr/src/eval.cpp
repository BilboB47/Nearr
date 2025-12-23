
#include "eval.hpp"
#include "bitboard_utils.hpp"


//============================================ewualcja na podstawie materia³u=====================================================


int evaluate_material(const Position& pos)
{	
	int value=0;

	//WHITE MATERIAL
	value += count_set_bits(pos.bitBoard[WHITE_PAWN]) * PAWN_VALUE;
	value += count_set_bits(pos.bitBoard[WHITE_KNIGHT]) * KNIGHT_VALUE;
	value += count_set_bits(pos.bitBoard[WHITE_BISHOP]) * BISHOP_VALUE;
	value += count_set_bits(pos.bitBoard[WHITE_ROOK]) * ROOK_VALUE;
	value += count_set_bits(pos.bitBoard[WHITE_QUEEN]) * QUEEN_VALUE;

	//BLACK MATEIRAL
	value -= count_set_bits(pos.bitBoard[BLACK_PAWN]) * PAWN_VALUE;
	value -= count_set_bits(pos.bitBoard[BLACK_KNIGHT]) * KNIGHT_VALUE;
	value -= count_set_bits(pos.bitBoard[BLACK_BISHOP]) * BISHOP_VALUE;
	value -= count_set_bits(pos.bitBoard[BLACK_ROOK]) * ROOK_VALUE;
	value -= count_set_bits(pos.bitBoard[BLACK_QUEEN]) * QUEEN_VALUE;

	return (pos.isWhiteMove? value : -value);
}



//===========================ewualcja na podstawie pozycji figury=====================================================
int pst[2][12][64];

void init_pst() {
	for (int phase = 0; phase < 2; ++phase) {
		for (int piece = 0; piece < 6; ++piece) {
			for (int sq = 0; sq < 64; ++sq) {
				// Kopiujemy bia³e (0-5)
				pst[phase][piece][sq] = Bonus_pst[phase][piece][sq];

				// Generujemy czarne (6-11)
				pst[phase][piece + 6][sq] = -Bonus_pst[phase][piece][sq ^ 56];
			}
		}
	}
}

int calculate_phase(const Position& pos) {
	int phase = 0;
	phase += count_set_bits(pos.bitBoard[WHITE_KNIGHT]) * 1;
	phase += count_set_bits(pos.bitBoard[BLACK_KNIGHT]) * 1;
	phase += count_set_bits(pos.bitBoard[WHITE_BISHOP]) * 1;
	phase += count_set_bits(pos.bitBoard[BLACK_BISHOP]) * 1;
	phase += count_set_bits(pos.bitBoard[WHITE_ROOK]) * 2;
	phase += count_set_bits(pos.bitBoard[BLACK_ROOK]) * 2;
	phase += count_set_bits(pos.bitBoard[WHITE_QUEEN]) * 4;
	phase += count_set_bits(pos.bitBoard[BLACK_QUEEN]) * 4;
	return phase;
}

int evaluate_pst(const Position& pos){

	int mg = 0;
	int eg = 0;


	int phase = calculate_phase(pos);
	if (phase > 24) phase = 24;//jakby bylo promocja

	for (int i = 0; i < 12; i++) {
		uint64_t bb_piece = pos.bitBoard[i];

		while (bb_piece) { //liczy dla danej figury
			uint8_t index = pop_lsb(&bb_piece);//obliczanie dla danego indexu
			mg += pst[0][i][index];
			eg += pst[1][i][index];
		}
	}

	int score=(eg * (24 - phase) + mg * phase) / 24;

	return (pos.isWhiteMove) ? score : -score;
}

//================================================SUMA EVAL===================================================
int evaluate_all(const Position& pos)
{
	return evaluate_material(pos) + evaluate_pst(pos);
}