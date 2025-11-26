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
	generatePawnMoves(pos,moves);
	generateKnightMoves(pos, moves);
	generateBishopMoves(pos, moves);
	generateRookMoves(pos, moves);
	generateQueenMoves(pos, moves);
	generateKingMoves(pos, moves);
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

 //56 57 58 59 60 61 62 63
 //48 49 50 51 52 53 54 55
 //40 41 42 43 44 45 46 47
 //32 33 34 35 36 37 38 39
 //24 25 26 27 28 29 30 31
 //16 17 18 19 20 21 22 23
 //8  9 10 11 12 13 14 15   
 //0  1  2  3  4  5  6  7


void generatePawnMoves(const Position& pos, std::vector<Move>& moves)
{
	//WHITE_PAWN - 0
	uint8_t index_piece = (pos.isWhiteMove) ? WHITE_PAWN : BLACK_PAWN;
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
				moves.push_back(Move(i, to, index_piece, NO_PIECE, promo));
			}

		//pojscie o 2 do przodu 
			if ((pos.isWhiteMove && i >= 8 && i <= 15) || (!pos.isWhiteMove && i >= 48 && i <= 55)) {
				int to2 = i + ((pos.isWhiteMove) ? 16 : -16);
				if ( !((pos.bitBoard[WHITE_ALL] | pos.bitBoard[BLACK_ALL]) & (1ULL << to)) &&
					 !((pos.bitBoard[WHITE_ALL] | pos.bitBoard[BLACK_ALL]) & (1ULL << to2)) ) { //sprawdza czy jeden i dwa przed nim cos stoi

					//dodanie ruchu o 2 do listy move
					moves.push_back(Move(i, to2, index_piece, NO_PIECE, NO_PIECE));
				}			
			}
			
		//bicie o jeden w prawo i lewo 
			int left = pos.isWhiteMove ? i + 7 : i - 9;
			int right = pos.isWhiteMove ? i + 9 : i - 7;

			uint8_t oponnent = pos.isWhiteMove ? BLACK_ALL : WHITE_ALL;

			if (i%8!=0) {//tylko poza lew¹ stron¹

				uint8_t promo = NO_PIECE;
				if ((pos.isWhiteMove && left >= 56) || (!pos.isWhiteMove && left <= 7)) {
					promo = ((pos.isWhiteMove) ? WHITE_QUEEN : BLACK_QUEEN);
				}

				if (pos.bitBoard[oponnent] & (1ULL << left)) {//sprawdza czy jest coœ do bicia
					moves.push_back(Move(i, left, index_piece, pos.piece_on_square(left), promo));
				}
			}

			if (i % 8 != 7) {//tylko poza prawa stron¹

				uint8_t promo = NO_PIECE;
				if ((pos.isWhiteMove && right >= 56) || (!pos.isWhiteMove && right <= 7)) {
					promo = ((pos.isWhiteMove) ? WHITE_QUEEN : BLACK_QUEEN);
				}

				if (pos.bitBoard[oponnent] & (1ULL << right)) {//sprawdza czy jest coœ do bicia
					moves.push_back(Move(i, right, index_piece, pos.piece_on_square(right),promo));
				}	
			}

				
			//MO¯NA DODAÆ enPassant
		}
	}
}
void generateKnightMoves(const Position& pos, std::vector<Move>& moves)
{
	uint8_t index_piece = (pos.isWhiteMove) ? WHITE_KNIGHT : BLACK_KNIGHT;
	uint8_t friendly = pos.isWhiteMove ? WHITE_ALL : BLACK_ALL;
	uint8_t oponnent = pos.isWhiteMove ? BLACK_ALL : WHITE_ALL;

	for (int i = 0; i < 64; i++) {
		if (pos.bitBoard[index_piece] & (1ULL << i)) {

			for (const int jump : knightAttacks[i]){ //patrze na jakie pola moze sie ruszyc skoczek
				if (pos.bitBoard[friendly] & (1ULL << jump))continue; //jesli jest tam ten sam kolor to nie ma co
				uint8_t piece = NO_PIECE;
				if (pos.bitBoard[oponnent] & (1ULL << jump)){ //jakiego piona zbije ten ruch
					piece = pos.piece_on_square(jump);
				}
				moves.push_back(Move(i, jump, index_piece, piece, NO_PIECE));
			}
		}
	}

}
void generateBishopMoves(const Position& pos, std::vector<Move>& moves)
{
	int index_piece = (pos.isWhiteMove) ? WHITE_BISHOP : BLACK_BISHOP;
	uint8_t friendly = pos.isWhiteMove ? WHITE_ALL : BLACK_ALL;
	uint8_t oponnent = pos.isWhiteMove ? BLACK_ALL : WHITE_ALL;
	int diagonal[4] = { 9,-9,7,-7 };//prawo góra | lewo dó³ | lewo góra | prawo dó³
	for (int i = 0; i < 64; i++) {
		if (pos.bitBoard[index_piece] & (1ULL << i)) {//jak jest goniec to zrobi
			for (int dir = 0; dir < 4; dir++) {//kazdy skoks po kolei
			int to = i;//pozycja startowa by robic skosy
				while (1){//obsluga danego skosku

					int col = to % 8;
					if ((dir == 0 || dir == 3) && col == 7) break;
					if ((dir == 1 || dir == 2) && col == 0) break;
					
					to += diagonal[dir];
					if (to < 0 || to > 63) break;


					if (pos.bitBoard[friendly] & (1ULL << to))break; //blokuje jakiœ 

					if (pos.bitBoard[oponnent] & (1ULL << to)) { //jakiego piona zbije ten ruch i przez to nie moze iœæ dalej
						uint8_t piece = pos.piece_on_square(to);
						moves.push_back(Move(i, to, index_piece, piece, NO_PIECE));
						break;
					}
					moves.push_back(Move(i, to, index_piece, NO_PIECE, NO_PIECE));
				}
			}
		}
	}
}
void generateRookMoves(const Position& pos, std::vector<Move>& moves)
{
	int index_piece = (pos.isWhiteMove) ? WHITE_ROOK : BLACK_ROOK;
	uint8_t friendly = pos.isWhiteMove ? WHITE_ALL : BLACK_ALL;
	uint8_t oponnent = pos.isWhiteMove ? BLACK_ALL : WHITE_ALL;
	int diagonal[4] = { 8,-8,1,-1 };//góra | dó³ | prawo | lewo

	for (int i = 0; i < 64; i++) {
		if (pos.bitBoard[index_piece] & (1ULL << i)) {//jak jest wie¿a to zrobi
			for (int dir = 0; dir < 4; dir++) {//kazdy kierunek po kolei
				int to = i;//pozycja startowa by robic kierunku
				while (1) {//obsluga danego kierunku
					int col = to % 8;
					if (dir == 2 && col == 7) break;
					if (dir == 3 && col == 0) break;

					to += diagonal[dir];
					if (to < 0 || to > 63) break;


					if (pos.bitBoard[friendly] & (1ULL << to))break; //blokuje jakiœ 

					if (pos.bitBoard[oponnent] & (1ULL << to)) { //jakiego piona zbije ten ruch i przez to nie moze iœæ dalej
						uint8_t piece = pos.piece_on_square(to);
						moves.push_back(Move(i, to, index_piece, piece, NO_PIECE));
						break;
					}
					moves.push_back(Move(i, to, index_piece, NO_PIECE, NO_PIECE));
				}

			}
		}
	}
}
void generateQueenMoves(const Position& pos, std::vector<Move>& moves)
{
	int index_piece = (pos.isWhiteMove) ? WHITE_QUEEN : BLACK_QUEEN;
	uint8_t friendly = pos.isWhiteMove ? WHITE_ALL : BLACK_ALL;
	uint8_t oponnent = pos.isWhiteMove ? BLACK_ALL : WHITE_ALL;
	int diagonal[8] = { 8,-8,1,-1 ,9,-9,7,-7 };//góra | dó³ | prawo | lewo  || goniec
	
	for (int i = 0; i < 64; i++) {
		if (pos.bitBoard[index_piece] & (1ULL << i)) {//jak jest
			for (int dir = 0; dir < 8; dir++) {
				int to = i;//pozycja startowa
				while (1) {//obsluga danego skosku
					int col = to % 8;

					if (dir <= 3){
						if (dir == 3 && col == 0) break;
						if (dir == 2 && col == 7) break;
					}
					else {
						if ((dir == 4 || dir == 7) && col == 7) break;
						if ((dir == 5 || dir == 6) && col == 0) break;
					}

					to += diagonal[dir];
					if (to < 0 || to > 63) break;

					if (pos.bitBoard[friendly] & (1ULL << to))break; //blokuje jakiœ 

					if (pos.bitBoard[oponnent] & (1ULL << to)) { //jakiego piona zbije ten ruch i przez to nie moze iœæ dalej
						uint8_t piece = pos.piece_on_square(to);
						moves.push_back(Move(i, to, index_piece, piece, NO_PIECE));
						break;
					}
					moves.push_back(Move(i, to, index_piece, NO_PIECE, NO_PIECE));
				}
			}
		}
	}

}
void generateKingMoves(const Position& pos, std::vector<Move>& moves)
{
	uint8_t index_piece = (pos.isWhiteMove) ? WHITE_KING : BLACK_KING;
	uint8_t friendly = pos.isWhiteMove ? WHITE_ALL : BLACK_ALL;
	uint8_t oponnent = pos.isWhiteMove ? BLACK_ALL : WHITE_ALL;

	for (int i = 0; i < 64; i++) {
		if (pos.bitBoard[index_piece] & (1ULL << i)) {

			for (const int jump : kingAttacks[i]) { //patrze na jakie pola moze sie ruszyc skoczek
				if (pos.bitBoard[friendly] & (1ULL << jump))continue; //jesli jest tam ten sam kolor to nie ma co
				uint8_t piece = NO_PIECE;
				if (pos.bitBoard[oponnent] & (1ULL << jump)) { //jakiego piona zbije ten ruch
					piece = pos.piece_on_square(jump);
				}
				moves.push_back(Move(i, jump, index_piece, piece, NO_PIECE));
			}
		}

		//
		//	ROSZADA
		//
	}

}


