#include "movegen.hpp"

std::array<std::vector<int>, 64> kingAttacks{};
std::array<std::vector<int>, 64> knightAttacks{};

void initAttackTables()
{
	initKingAttacks();
	initKnightAttacks();
}

void initKingAttacks()
{
	int jumps[8][2] = {
		{-1,1},{0,1},{1,1},
		{-1,0},{1,0},
		{-1,-1},{0,-1},{1,-1}
	};

	for (int i = 0; i < 64; i++) {
		int col = i % 8;
		int row = i / 8;

		for (int j = 0; j < 8; j++) {
			int cur_col = jumps[j][0] + col;
			int cur_row = jumps[j][1] + row;

			if ((0 <= cur_col && cur_col <= 7) && (0 <= cur_row && cur_row <= 7)) {
				kingAttacks[i].push_back(cur_row * 8 + cur_col);
			}
		}
	}
}//wypisuje tablice legalnych ruchów dla króla w wybranej jego pozycji

void initKnightAttacks()
{	
	int jumps[8][2] = { //jaki ruch moze byæ wykonany L
		{2,1},{2,-1},{1,2},{1,-2},
		{-2,1},{-2,-1},{-1,2},{-1,-2}
	};

	for (int i = 0; i < 64; i++){
		int col = i % 8;
		int row = i / 8;
		
		for (int j = 0; j < 8;j++) {
			int cur_col = jumps[j][0] + col;
			int cur_row = jumps[j][1] + row;

			if ((0 <= cur_col && cur_col <= 7) && (0 <= cur_row && cur_row <= 7)) { //niewychodzi poza plansze?
				knightAttacks[i].push_back(cur_row*8+cur_col);
			}
		}
	}
}
//wypisuje tablice legalnych ruchów dla szkocza w wybranej jego pozycji

std::vector<Move> generateMoves(const Position& pos)
{
	std::vector<Move> moves;
	
	return moves;
}

//uint8_t from;		//0-63 <=> a1-h8
//uint8_t to;			//0-63 <=> a1-h8
//uint8_t piece;		//ruch
//uint8_t captured;	//zbito 0-11 , 
//uint8_t promotion;

//uint64_t board[14];  
//bool isWhiteMove;
//uint8_t castlingRights;
//uint8_t enPassantSquare;
//uint16_t moveNumber;

void generatePawnMoves(const Position& pos, std::vector<Move>& moves)
{
	//WHITE_PAWN - 0
	int index_piece = (pos.isWhiteMove) ? WHITE_PAWN : BLACK_PAWN;
	for (int i = 0; i < 64; i++) {
		if (pos.bitBoard[index_piece] & (1ULL << i)) {//czy jest na tym polu pion?

		//pojscie o 1 do przodu
			int to = i + ((pos.isWhiteMove) ? 8 : -8);
			if (!((pos.bitBoard[WHITE_ALL] | pos.bitBoard[BLACK_ALL]) & (1ULL << to))) { //sprawdza czy jeden przed nim cos stoi
				
				uint8_t promo = NO_PIECE;
				if ((pos.isWhiteMove && to >= 56) || (!pos.isWhiteMove && to <= 7)) {
					promo = ((pos.isWhiteMove) ? WHITE_QUEEN : BLACK_QUEEN);
				}
				//dodanie ruchu o 1 do listy move
				moves.push_back(Move(i, to, (pos.isWhiteMove ? WHITE_PAWN : BLACK_PAWN), NO_PIECE, promo));
			}

		//pojscie o 2 do przodu 
			if ((pos.isWhiteMove && i >= 8 && i <= 15) || (!pos.isWhiteMove && i >= 48 && i <= 55)) {
				int to2 = i + ((pos.isWhiteMove) ? 16 : -16);
				if ( !((pos.bitBoard[WHITE_ALL] | pos.bitBoard[BLACK_ALL]) & (1ULL << to)) &&
					 !((pos.bitBoard[WHITE_ALL] | pos.bitBoard[BLACK_ALL]) & (1ULL << to2)) ) { //sprawdza czy jeden i dwa przed nim cos stoi

					//dodanie ruchu o 2 do listy move
					moves.push_back(Move(i, to2, (pos.isWhiteMove ? WHITE_PAWN : BLACK_PAWN), NO_PIECE, NO_PIECE));
				}			
			}
			
		//bicie o jeden w prawo i lewo 
			int left = pos.isWhiteMove ? i + 7 : i - 9;
			int right = pos.isWhiteMove ? i + 9 : i - 7;

			int oponnent = pos.isWhiteMove ? BLACK_ALL : WHITE_ALL;

			if (i%8!=0) {//tylko poza lew¹ stron¹

				uint8_t promo = NO_PIECE;
				if ((pos.isWhiteMove && left >= 56) || (!pos.isWhiteMove && left <= 7)) {
					promo = ((pos.isWhiteMove) ? WHITE_QUEEN : BLACK_QUEEN);
				}

				if (pos.bitBoard[oponnent] & (1ULL << left)) {//sprawdza czy jest coœ do bicia
					moves.push_back(Move(i, left, (pos.isWhiteMove ? WHITE_PAWN : BLACK_PAWN), pos.piece_on_square(left), promo));
				}
			}

			if (i % 8 != 7) {//tylko poza prawa stron¹

				uint8_t promo = NO_PIECE;
				if ((pos.isWhiteMove && right >= 56) || (!pos.isWhiteMove && right <= 7)) {
					promo = ((pos.isWhiteMove) ? WHITE_QUEEN : BLACK_QUEEN);
				}

				if (pos.bitBoard[oponnent] & (1ULL << right)) {//sprawdza czy jest coœ do bicia
					moves.push_back(Move(i, right, (pos.isWhiteMove ? WHITE_PAWN : BLACK_PAWN), pos.piece_on_square(right),promo));
				}
			}

				
			//MO¯NA DODAÆ enPassant
		}
	}
}

