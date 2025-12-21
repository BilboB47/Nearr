
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


//===========================ewualcja na podstawie pozycji piona=====================================================
inline int get_pst_value(int square, Color color, const int* table){ //revers for black table
	return (color == WHITE) ? table[square] : table[square ^ 56];
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
double tapered_eval(const Position& pos)
{	
	int phase = calculate_phase(pos);
	return 0.0;
}

int compute_midgame_score(const Position& pos)
{
	int score = 0;

}

int compute_endgame_score(const Position& pos)
{
	return 0;
}
