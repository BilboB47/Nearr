
#include "eval.hpp"
#include "bitboard_utils.hpp"


//============================================ewualcja na podstawie materia³u=====================================================
//jest to obiektywne poniewa¿ jest to do inicjacji
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

	return value;
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
	
	for (int i = 0; i < 12; i++) {
		phase += count_set_bits(pos.bitBoard[i]) * piecePhase[i];
	}
	
	return phase;
}

int evaluate_pstMG(const Position& pos){

	int mg = 0;

	for (int i = 0; i < 12; i++) {
		uint64_t bb_piece = pos.bitBoard[i];

		while (bb_piece) { //liczy dla danej figury
			uint8_t index = pop_lsb(&bb_piece);//obliczanie dla danego indexu
			mg += pst[0][i][index];
		}
	}

	return mg;
}
int evaluate_pstEG(const Position& pos) {

	int eg = 0;

	for (int i = 0; i < 12; i++) {
		uint64_t bb_piece = pos.bitBoard[i];

		while (bb_piece) { //liczy dla danej figury
			uint8_t index = pop_lsb(&bb_piece);//obliczanie dla danego indexu
			eg += pst[1][i][index];
		}
	}

	return eg;
}

//===========================================================================
int evaluate(const Position& pos) {
	int mgScore = pos.currentEval.material + pos.currentEval.pstMG;
	int egScore = pos.currentEval.material + pos.currentEval.pstEG;

	int phase = pos.phase;
	if (phase > 24) phase = 24;
	else if (phase < 0) phase = 0;

	int eval = (mgScore * phase + egScore * (24 - phase)) / 24;

	return (pos.isWhiteMove) ? eval : -eval;
}






