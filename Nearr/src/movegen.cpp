#include "movegen.hpp"


//====================================FUNKCJE BITOWE=============================================================
inline int pop_lsb(uint64_t* bitboard)
{
	unsigned long int index;
	if (_BitScanForward64(&index, *bitboard)){
		*bitboard &= *bitboard - 1; //usuwa te nie znaczace czyli 11100 -> 11000 czyli ,za 1 lsb da 0
		return (int)index;
	}
	return -1;
}
inline int get_lsb(uint64_t* bitboard)
{
	unsigned long int index;
	if (_BitScanForward64(&index, *bitboard)) {
		return (int)index;
	}
	return -1;
}
int count_set_bits(uint64_t bb) {
	return (int)__popcnt64(bb);
}

//====================================GENEROWANIE LICZB LOSOWYCH=============================================================
uint64_t random_uint64(std::mt19937_64& generator, std::uniform_int_distribution<uint64_t>& dystrybucja) {
	return dystrybucja(generator);
}
uint64_t generate_filtered_magic_candidate(std::mt19937_64& generator, std::uniform_int_distribution<uint64_t>& dystrybucja) {
	uint64_t r1 = random_uint64(generator, dystrybucja);
	uint64_t r2 = random_uint64(generator, dystrybucja);
	uint64_t r3 = random_uint64(generator, dystrybucja);

	return r1 & r2 & r3;
}


//====================================GENEROWANIE WARIANTÓW=============================================================
void generate_blocker_patterns(uint64_t mask, int num_bits, std::vector<uint64_t>& patterns) {//generuje wszystkie mo¿liwe ustawienie bloków


	int *relevant_squares = new int[num_bits];
	int index = 0;
	for (int i = 0; i < 64; i++) {
		if (mask & (1ULL << i)){
			relevant_squares[index] = i; //watchout 
			index++;
		}
	}

	patterns.clear();
	
	int num_patterns = (1ULL) << num_bits;
	for (int i = 0; i < num_patterns; i++) { //licznik binarny przez którego np.11001 | 1 - oznacza ustaw , 0 - skip
		uint64_t blocker = (0ULL);
		
		for (int j = 0; j < num_bits;j++) { //patrzy czy 1 jest na tej pozycji jeœli tak to ustaw to na podstawie  relevant_squares[j] w prawidzej masce
			if (i & (1 << j)) {

				int square_index = relevant_squares[j];

				blocker |= (1ULL << square_index);
			}
		}


		patterns.emplace_back(blocker);
	}

	delete[] relevant_squares;
}

//===================================SPRAWDZANIE CZY POLE JEST ATAKOWANE====================================================
bool is_square_attacked(const Position& pos, int square, Color attack_by){
	if (is_pawn_attacked(pos, square, attack_by))return true;
	if (is_knight_attacked(pos, square, attack_by))return true;
	if (is_king_attacked(pos, square, attack_by))return true;
	if (is_bishop_or_queen_attacked(pos, square, attack_by))return true;
	if (is_rook_or_queen_attacked(pos, square, attack_by))return true;

	return false;
}
bool is_in_check(const Position& pos) {//sprawdza czy ten kto robi ruch otrzymuje szacha
	
	uint8_t piece = (pos.isWhiteMove) ?  WHITE_KING : BLACK_KING;

	Color attacking_color = (pos.isWhiteMove) ? BLACK : WHITE;

	uint64_t king_bb = pos.bitBoard[piece];
	int square = get_lsb(&king_bb);

	return is_square_attacked(pos, square, attacking_color);
}

bool is_pawn_attacked(const Position& pos, int square, Color attack_by) {
	
	//atakuj¹cy
	uint8_t attacking_piece = (attack_by==WHITE) ? WHITE_PAWN : BLACK_PAWN; 
	uint64_t attacking_pawn = pos.bitBoard[attacking_piece];

	//atakowany
	uint8_t index_table = (attack_by == WHITE) ? 1 : 0;

	//odwrocenie ataku
	uint64_t moves_to = pawnAttacks[index_table][square];
	
	return (attacking_pawn & moves_to) != 0;
}
bool is_king_attacked(const Position& pos, int square, Color attack_by) {
	//atakuj¹cy
	uint8_t attacking_piece = (attack_by == WHITE) ? WHITE_KING : BLACK_KING;
	uint64_t attacking_king = pos.bitBoard[attacking_piece];

	//odwrocenie ataku
	uint64_t moves_to = kingAttacks[square];

	return (attacking_king & moves_to) != 0;
}
bool is_knight_attacked(const Position& pos, int square, Color attack_by) {
	//atakuj¹cy
	uint8_t attacking_piece = (attack_by == WHITE) ? WHITE_KNIGHT : BLACK_KNIGHT;
	uint64_t attacking_knight = pos.bitBoard[attacking_piece];

	//odwrocenie ataku
	uint64_t moves_to = knightAttacks[square];

	return (attacking_knight & moves_to) != 0;
}
bool is_bishop_or_queen_attacked(const Position& pos, int square, Color attack_by) {
	
	//atakuj¹cy
	uint8_t attacking_bishop = (attack_by == WHITE) ? WHITE_BISHOP : BLACK_BISHOP;
	uint8_t attacking_queen = (attack_by == WHITE) ? WHITE_QUEEN : BLACK_QUEEN;
	uint64_t attacking_board = pos.bitBoard[attacking_bishop] | pos.bitBoard[attacking_queen];

	//odwrocenie ataku	
	uint64_t all = pos.getAllPieces();
	uint64_t moves_to = get_bishop_attacks(square, all);

	return (attacking_board & moves_to) != 0;
}
bool is_rook_or_queen_attacked(const Position& pos, int square, Color attack_by){
	//atakuj¹cy
	uint8_t attacking_rook = (attack_by == WHITE) ? WHITE_ROOK : BLACK_ROOK;
	uint8_t attacking_queen = (attack_by == WHITE) ? WHITE_QUEEN : BLACK_QUEEN;
	uint64_t attacking_board = pos.bitBoard[attacking_rook] | pos.bitBoard[attacking_queen];

	//odwrocenie ataku	
	uint64_t all = pos.getAllPieces();
	uint64_t moves_to = get_rook_attacks(square, all);

	return (attacking_board & moves_to) != 0;
}


//===============================================================================================================
//--------------------------------GENEROWANIE PSEUDORUCHÓW FIGUR---------------------------------------------------------------------------------------------- 
//===============================================================================================================


//====================================WIE¯A=========================================================================
uint64_t RookMasks[64]{};//blokerów dla danego pola | B
uint64_t RookMagicNumbers[64]{};//magiczna liczba dla danego pola  | M
uint64_t RookShifts[64]{};//przesuniecie bitowe dla danego pola | S 
uint64_t RookOffsets[64]{};//index startowy segmentu dla danego pola w RookAttackTable 
uint64_t RookAttackTable[ROOK_ATTACK_TABLE_SIZE]{};//wszystkie mo¿liwe ataki i jest podzielona na segmenty dla danego pola|a1|a2|

//WZÓR index = (B * M) >> S
void initRookMasks(){
	
	for (int i = 0; i < 64; i++){
		uint64_t masc = 0ULL;
		int row = i / 8;
		int col = i % 8;
		
		for (int j = col+1; j < 7; j++)masc |= ((1ULL) << (row*8 + j));//prawo
		for (int j = col-1; j > 0; j--)masc |= ((1ULL) << (row * 8 + j));//lewo
		for (int j = row+1; j < 7; j++)masc |= ((1ULL) << (j * 8 + col));//góra
		for (int j = row-1; j > 0; j--)masc |= ((1ULL) << (j * 8 + col));//dó³

		RookMasks[i] = masc;
	}
}//generuje maske blokerow
uint64_t get_rook_attacks_slow(int square, uint64_t blockers) { 
	uint64_t moves=0ULL;
	int row = square / 8;
	int col = square % 8;

	for (int j = col + 1; j < 8; j++){//prawo
		moves |= ((1ULL) << (row * 8 + j));
		if (blockers & ((1ULL) << (row * 8 + j)))break;
	}
	for (int j = col - 1; j >= 0; j--) {//lewo
		moves |= ((1ULL) << (row * 8 + j));
		if (blockers & ((1ULL) << (row * 8 + j)))break;
	}
	for (int j = row + 1; j < 8; j++) {//góra
		moves |= ((1ULL) << (j * 8 + col));
		if (blockers & ((1ULL) << (j * 8 + col)))break;
	}
	for (int j = row - 1; j >= 0; j--) {//dó³
		moves |= ((1ULL) << (j * 8 + col));
		if (blockers & ((1ULL) << (j * 8 + col)))break;
	}


	return moves;
}//na podstawie blockers robi gdzie mo¿na iœæ
void initRookMagics() {	

	initRookMasks();

	int segment_start = 0;
	for (int i = 0; i < 64; i++) { //wyliczanie segmentów
		RookOffsets[i] = segment_start;

		//jaki rozmiar bedzie mial segment
		int num_bits = count_set_bits(RookMasks[i]);
		int num_patterns = (1ULL) << num_bits;

		RookShifts[i] = 64 - num_bits;	

		//o ile dalej bedzie w takim razie kolejny
		segment_start += num_patterns;
	}

	//RookAttackTable[ROOK_ATTACK_TABLE_SIZE]  czy dynamicznie alkowaæ rozmiar?

	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<uint64_t> distrib;

	for (int i = 0; i < 64; i++) {
		//wszystkie mo¿liwe ustawienia bloków
		std::vector<uint64_t> patterns;
		generate_blocker_patterns(RookMasks[i], count_set_bits(RookMasks[i]), patterns); //B

		std::vector<uint64_t> attacks; // A
		// Ataki dla ka¿dego ukladu bloków 
		for (const auto& blocker_key : patterns) {
			attacks.emplace_back(get_rook_attacks_slow(i, blocker_key));
		}

		uint64_t magic_candidate;
		int R = count_set_bits(RookMasks[i]); // R
		int shift = RookShifts[i];            // S
		int num_patterns = patterns.size();

		std::vector<uint64_t> used_attacks(num_patterns, 0ULL); //sprawdzanie kolizji
		
		bool collison;
		do{
			std::fill(used_attacks.begin(), used_attacks.end(), 0ULL);
			collison = false;
			magic_candidate = generate_filtered_magic_candidate(gen, distrib);

			for (int j = 0; j < num_patterns; j++){
				
				uint64_t blocker = patterns[j];
				uint64_t atack_value = attacks[j];
				//WZÓR index = (B * M) >> S
				uint64_t hash_index = (blocker * magic_candidate) >> shift;

				//sprawdzanie kolizji
				if (used_attacks[hash_index] == (0ULL)){
					used_attacks[hash_index] = atack_value;
				}else if (used_attacks[hash_index] != atack_value) {
					// Jest zajête i wartoœæ jest RÓ¯NA - to jest PRAWDZIWA kolizja!
					collison = true;
					break;
				}
			}


		} while (collison);


		

		RookMagicNumbers[i] = magic_candidate;
		uint64_t offset = RookOffsets[i];

		for (int j = 0; j < num_patterns; ++j) {
			uint64_t blocker = patterns[j];

			uint64_t final_hash_index = (blocker * magic_candidate) >> shift;

			RookAttackTable[offset + final_hash_index] = used_attacks[final_hash_index];
		}

	}
}

uint64_t get_rook_attacks(int square, uint64_t board) {

	//co blokuje wie¿e na tym polu
	uint64_t blockers = board & RookMasks[square];

	// jaki jest index haszujacy
	uint64_t magic = RookMagicNumbers[square];
	uint64_t shift = RookShifts[square];
	uint64_t hash_index = (blockers * magic) >> shift;

	//faktyczny adres
	uint64_t offset = RookOffsets[square];
	uint64_t final_index = offset + hash_index;

	//gdzie mozna isc bitboard
	return RookAttackTable[final_index];
}
void generateRookMoves(const Position& pos, std::vector<Move>& moves) {
	uint64_t all = pos.getAllPieces();
	uint64_t friendly = pos.getAllFriendlyPieces();

	uint8_t piece = (pos.isWhiteMove) ? WHITE_ROOK : BLACK_ROOK;
	uint64_t rooks = pos.bitBoard[piece];

	while (rooks) { //liczy dla danego goñca
		int index_rook = pop_lsb(&rooks);//okreœla jego pole i usuwa jest puli

		uint64_t moves_to = get_rook_attacks(index_rook, all);
		moves_to &= (~friendly); //odejmuje pozycje gdzie s¹ friendly figury

		while (moves_to) {
			int index_move = pop_lsb(&moves_to);

			uint8_t captured = pos.piece_on_square(index_move);
			moves.emplace_back(Move(index_rook, index_move, piece, captured));
		}
	}


}


//====================================GONIEC=========================================================================
uint64_t BishopMasks[64]{};
uint64_t BishopMagicNumbers[64]{};
uint64_t BishopShifts[64]{};
uint64_t BishopOffsets[64]{};
uint64_t BishopAttackTable[BISHOP_ATTACK_TABLE_SIZE]{};

//WZÓR index = (B * M) >> S
void initBishopMasks(){

	for (int i = 0; i < 64; i++) {
		uint64_t masc = 0ULL;
		int row = i / 8;
		int col = i % 8;

		for (int j = 1; (row+j<7)&&(col+j<7); j++) masc |= ((1ULL) << ((row+j)* 8 + col+j));//góra prawo 
		for (int j = 1; (row+j<7)&&(col-j>0); j++) masc |= ((1ULL) << ((row+j)* 8 + col-j));//góra lewo 
		for (int j = 1; (row-j>0)&&(col+j<7); j++) masc |= ((1ULL) << ((row-j)* 8 + col+j));//dó³ prawo 
		for (int j = 1; (row-j>0)&&(col-j>0); j++) masc |= ((1ULL) << ((row-j)* 8 + col-j));//dó³ lewo 

		BishopMasks[i] = masc;
	}
};
uint64_t get_bishop_attacks_slow(int square, uint64_t blockers) {
	uint64_t moves = 0ULL;
	int row = square / 8;
	int col = square % 8;

	for (int j = 1; (row + j < 8) && (col + j < 8); j++) {//góra prawo
		moves |= ((1ULL) << ((row + j) * 8 + col + j));
		if (blockers & ((1ULL) << ((row + j) * 8 + col + j)))break;
	}

	for (int j = 1; (row + j < 8) && (col - j >= 0); j++) {//góra lewo 
		moves |= ((1ULL) << ((row + j) * 8 + col - j));
		if (blockers & ((1ULL) << ((row + j) * 8 + col - j)))break;
	}

	for (int j = 1; (row - j >= 0) && (col + j < 8); j++) {//dó³ prawo
		moves |= ((1ULL) << ((row - j) * 8 + col + j));
		if (blockers & ((1ULL) << ((row - j) * 8 + col + j)))break;
	}

	for (int j = 1; (row - j >= 0) && (col - j >= 0); j++) {//dó³ lewo 
		moves |= ((1ULL) << ((row - j) * 8 + col - j));
		if (blockers & ((1ULL) << ((row - j) * 8 + col - j)))break;
	}

	return moves;
};
void initBishopMagics(){
		initBishopMasks();

		int segment_start = 0;
		for (int i = 0; i < 64; i++) { //wyliczanie segmentów
			BishopOffsets[i] = segment_start;

			//jaki rozmiar bedzie mial segment
			int num_bits = count_set_bits(BishopMasks[i]);
			int num_patterns = (1ULL) << num_bits;

			BishopShifts[i] = 64 - num_bits;

			//o ile dalej bedzie w takim razie kolejny
			segment_start += num_patterns;
		}

		//RookAttackTable[ROOK_ATTACK_TABLE_SIZE]  czy dynamicznie alkowaæ rozmiar?

		std::random_device rd;
		std::mt19937_64 gen(rd());
		std::uniform_int_distribution<uint64_t> distrib;

		for (int i = 0; i < 64; i++) {
			//wszystkie mo¿liwe ustawienia bloków
			std::vector<uint64_t> patterns;
			generate_blocker_patterns(BishopMasks[i], count_set_bits(BishopMasks[i]), patterns); //B

			std::vector<uint64_t> attacks; // A
			// Ataki dla ka¿dego ukladu bloków 
			for (const auto& blocker_key : patterns) {
				attacks.emplace_back(get_bishop_attacks_slow(i, blocker_key));
			}

			uint64_t magic_candidate;
			int R = count_set_bits(BishopMasks[i]); // R
			int shift = BishopShifts[i];            // S
			int num_patterns = patterns.size();

			std::vector<uint64_t> used_attacks(num_patterns, 0ULL); //sprawdzanie kolizji

			bool collison;
			do {
				std::fill(used_attacks.begin(), used_attacks.end(), 0ULL);
				collison = false;
				magic_candidate = generate_filtered_magic_candidate(gen, distrib);

				for (int j = 0; j < num_patterns; j++) {

					uint64_t blocker = patterns[j];
					uint64_t atack_value = attacks[j];
					//WZÓR index = (B * M) >> S
					uint64_t hash_index = (blocker * magic_candidate) >> shift;

					//sprawdzanie kolizji
					if (used_attacks[hash_index] == (0ULL)) {
						used_attacks[hash_index] = atack_value;
					}
					else if (used_attacks[hash_index] != atack_value) {
						// Jest zajête i wartoœæ jest RÓ¯NA - to jest PRAWDZIWA kolizja!
						collison = true;
						break;
					}
				}


			} while (collison);




			BishopMagicNumbers[i] = magic_candidate;
			uint64_t offset = BishopOffsets[i];

			for (int j = 0; j < num_patterns; ++j) {
				uint64_t blocker = patterns[j];

				uint64_t final_hash_index = (blocker * magic_candidate) >> shift;

				BishopAttackTable[offset + final_hash_index] = used_attacks[final_hash_index];
			}

		}
	};

uint64_t get_bishop_attacks(int square, uint64_t board) {
	//co blokuje goñca na tym polu
	uint64_t blockers = board & BishopMasks[square];

	// jaki jest index haszujacy
	uint64_t magic = BishopMagicNumbers[square];
	uint64_t shift = BishopShifts[square];
	uint64_t hash_index = (blockers * magic) >> shift;

	//faktyczny adres
	uint64_t offset = BishopOffsets[square];
	uint64_t final_index = offset + hash_index;

	//co pod jest pod nim
	return BishopAttackTable[final_index];
};
void generateBishopMoves(const Position& pos, std::vector<Move>& moves) {
	
	uint64_t all = pos.getAllPieces();
	uint64_t friendly = pos.getAllFriendlyPieces();
	
	uint8_t piece = (pos.isWhiteMove) ? WHITE_BISHOP : BLACK_BISHOP;
	uint64_t bishops = pos.bitBoard[piece];

	while (bishops){ //liczy dla danego goñca
		int index_bishop = pop_lsb(&bishops);//okreœla jego pole i usuwa jest puli
		
		uint64_t moves_to = get_bishop_attacks(index_bishop, all);
		moves_to &= (~friendly); //odejmuje pozycje gdzie s¹ friendly figury

		while (moves_to) {
			int index_move = pop_lsb(&moves_to);

			uint8_t captured = pos.piece_on_square(index_move);
			moves.emplace_back(Move(index_bishop,index_move,piece, captured));
		}
	}

};

//====================================SKOCZEK=============================================================
uint64_t knightAttacks[64]{};
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
				knightAttacks[i] |= ((1ULL)<<(cur_row*8+cur_col));
			}
		}
	}

}

void generateKnightMoves(const Position& pos, std::vector<Move>& moves) {
	uint64_t all = pos.getAllPieces();
	uint64_t friendly = pos.getAllFriendlyPieces();

	uint8_t piece = (pos.isWhiteMove) ? WHITE_KNIGHT : BLACK_KNIGHT;
	uint64_t knight = pos.bitBoard[piece];

	while (knight) { //liczy dla danego goñca
		int index_knight = pop_lsb(&knight);//okreœla jego pole i usuwa jest puli

		uint64_t moves_to = knightAttacks[index_knight];
		moves_to &= (~friendly); //odejmuje pozycje gdzie s¹ friendly figury

		while (moves_to) {
			int index_move = pop_lsb(&moves_to);

			uint8_t captured = pos.piece_on_square(index_move);
			moves.emplace_back(Move(index_knight, index_move, piece, captured));
		}
	}
}

//====================================KRÓL=============================================================
uint64_t kingAttacks[64]{};

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
				kingAttacks[i] |= ((1ULL) << (cur_row * 8 + cur_col));
			}
		}
	}
}
void generateCastlingMoves(const Position& pos, std::vector<Move>& moves)
{
	if (is_in_check(pos))return;
		
	uint64_t all = pos.bitBoard[BLACK_ALL] | pos.bitBoard[WHITE_ALL];
		//roszada dla bia³ych
		const Color ENEMY_COLOR = (pos.isWhiteMove) ? BLACK : WHITE;
		if (pos.isWhiteMove) {
			//krótka
			uint64_t F1_G1_BB = ((1ULL) << F1) | ((1ULL) << G1);
			if ((pos.castlingRights & WK) && !(all & F1_G1_BB) &&
				!is_square_attacked(pos, F1, ENEMY_COLOR) &&
				!is_square_attacked(pos, G1, ENEMY_COLOR)) {

				moves.emplace_back(Move(E1, G1, WHITE_KING, NO_PIECE, FLAG_CASTLE_KINGSIDE));
			}

			//d³uga
			uint64_t B1_C1_D1_BB = ((1ULL) << B1) | ((1ULL) << C1) | ((1ULL) << D1);
			if ((pos.castlingRights & WQ) && !(all & B1_C1_D1_BB) &&
				!is_square_attacked(pos, D1, ENEMY_COLOR) &&
				!is_square_attacked(pos, C1, ENEMY_COLOR)) {

				moves.emplace_back(Move(E1, C1, WHITE_KING, NO_PIECE, FLAG_CASTLE_QUEENSIDE));
			}
		}
		//czarna
		else {

			// Krótka 
			uint64_t F8_G8_BB = ((1ULL) << F8) | ((1ULL) << G8);
			if ((pos.castlingRights & BK) && !(all & F8_G8_BB) &&
				!is_square_attacked(pos, F8, ENEMY_COLOR) &&
				!is_square_attacked(pos, G8, ENEMY_COLOR)) {

				moves.emplace_back(Move(E8, G8, BLACK_KING, NO_PIECE, FLAG_CASTLE_KINGSIDE));
			}

			// D³uga 
			uint64_t B8_C8_D8_BB = ((1ULL) << B8) | ((1ULL) << C8) | ((1ULL) << D8);
			if ((pos.castlingRights & BQ) && !(all & B8_C8_D8_BB) &&
				!is_square_attacked(pos, D8, ENEMY_COLOR) &&
				!is_square_attacked(pos, C8, ENEMY_COLOR)) {

				moves.emplace_back(Move(E8, C8, BLACK_KING, NO_PIECE, FLAG_CASTLE_QUEENSIDE));
			}
		}

}

void generateKingMoves(const Position& pos, std::vector<Move>& moves) {
	uint64_t all = pos.getAllPieces();
	uint64_t friendly = pos.getAllFriendlyPieces();

	uint8_t piece = (pos.isWhiteMove) ? WHITE_KING : BLACK_KING;
	uint64_t king = pos.bitBoard[piece];

	int index_king = get_lsb(&king);//okreœla jego pole i usuwa jest puli

	uint64_t moves_to = kingAttacks[index_king];
	moves_to &= (~friendly); //odejmuje pozycje gdzie s¹ friendly figury

	while (moves_to) {
		int index_move = pop_lsb(&moves_to);

		uint8_t captured = pos.piece_on_square(index_move);
		moves.emplace_back(Move(index_king, index_move, piece, captured));
	}

	generateCastlingMoves(pos,moves);

}

//====================================PION=============================================================
uint64_t pawnAttacks[2][64]{};
void initPawnAttacks(){
	std::fill(&pawnAttacks[0][0], &pawnAttacks[0][0] + 2 * 64, 0ULL);
	const uint64_t A_FILE = 0x0101010101010101ULL; // Kolumna A
	const uint64_t H_FILE = 0x8080808080808080ULL; // Kolumna H

	//WHITE
	for (int i = 0; i <= 55; i++) {//nie biore ostatniej lini bo jak sie na niej jest to juz nie ma co bic
		uint64_t start_square = (1ULL << i);
		//zamiast start_square&A_FILE mo¿na i%8 != 0 (ale operacje % s¹ bardzo kosztowne jednak init sie robi tylko raz na poczatku) 
		if (!(start_square&A_FILE)) { //nie jest na lewej krawêdzi planszy 
			pawnAttacks[WHITE][i] |= start_square << 7;
		}
		if (!(start_square & H_FILE)) { //nie jest na prawej krawêdzi planszy 
			pawnAttacks[WHITE][i] |= start_square << 9;
		}
	}
	
	//BLACK
	for (int i = 8; i < 64; i++) {
		uint64_t start_square = (1ULL << i);
		
		if (!(start_square & A_FILE)) { //nie jest na lewej krawêdzi planszy 
			pawnAttacks[BLACK][i] |= start_square >> 9;
		}
		if (!(start_square & H_FILE)) { //nie jest na prawej krawêdzi planszy 
			pawnAttacks[BLACK][i] |= start_square >> 7;
		}
	}

}

uint64_t pawnSingleMoves[2][64]{};
uint64_t pawnDoubleMoves[2][64]{};
void initPawnMoves() {

	std::fill(&pawnSingleMoves[0][0], &pawnSingleMoves[0][0] + 2 * 64, 0ULL);
	std::fill(&pawnDoubleMoves[0][0], &pawnDoubleMoves[0][0] + 2 * 64, 0ULL);

	//WHITE
	const uint64_t WHITE_START = 0x000000000000FF00ULL;
	for (int i = 0; i < 64; i++){
		uint64_t start_square = (1ULL << i);

		if (i <= 55)pawnSingleMoves[WHITE][i] = start_square << 8;
		if (start_square & WHITE_START)pawnDoubleMoves[WHITE][i] |= start_square << 16;
	}

	//BLACK
	const uint64_t BLACK_START = 0x00FF000000000000;
	for (int i = 0; i < 64; i++) {
		uint64_t start_square = (1ULL << i);

		if (i >= 8)pawnSingleMoves[BLACK][i] = start_square >> 8;
		if (start_square & BLACK_START)pawnDoubleMoves[BLACK][i] |= start_square >> 16;
	}
}

void initPawnTables() {
	initPawnAttacks();
	initPawnMoves();
}

void generatePawnMoves(const Position& pos, std::vector<Move>& moves){
	uint64_t all = pos.getAllPieces();
	uint64_t friendly = pos.getAllFriendlyPieces();
	uint64_t enemy = pos.getAllEnemyPieces();

	uint8_t piece = (pos.isWhiteMove) ? WHITE_PAWN : BLACK_PAWN;
	uint8_t index_table = (pos.isWhiteMove) ? WHITE : BLACK;
	uint64_t pawns = pos.bitBoard[piece];

	uint64_t RANK_1 = 0x00000000000000FFULL;
	uint64_t RANK_8 = 0xFF00000000000000ULL;
	const uint64_t PROMOTION_RANK = pos.isWhiteMove ? RANK_8 : RANK_1;

	while (pawns) { //pêtla ob³ugi ka¿dego piona
		int index_pawn = pop_lsb(&pawns);

		//=========================BICIE=======================================
		uint64_t capture_targets = pawnAttacks[index_table][index_pawn];
		capture_targets &= (~friendly); //odejmuje pozycje gdzie s¹ friendly figury
		capture_targets &= enemy;

		while (capture_targets) {
			int index_move = pop_lsb(&capture_targets);

			uint64_t target_square_bb = 1ULL << index_move;
			uint8_t captured = pos.piece_on_square(index_move);
			
			if (target_square_bb & PROMOTION_RANK){
				moves.emplace_back(Move(index_pawn, index_move, piece, captured,FLAG_PROMOTION, PROMOTION_BISHOP));
				moves.emplace_back(Move(index_pawn, index_move, piece, captured,FLAG_PROMOTION, PROMOTION_KNIGHT));
				moves.emplace_back(Move(index_pawn, index_move, piece, captured,FLAG_PROMOTION, PROMOTION_QUEEN));
				moves.emplace_back(Move(index_pawn, index_move, piece, captured,FLAG_PROMOTION, PROMOTION_ROOK));
			}else {
				moves.emplace_back(Move(index_pawn, index_move, piece, captured));
			}
		}

		//==========================ruch do przodu======================================
		uint64_t single_push = pawnSingleMoves[index_table][index_pawn];
		uint64_t double_push = pawnDoubleMoves[index_table][index_pawn];

		uint64_t push_one = single_push & ~all;

		if (push_one) {

			int index_move = get_lsb(&push_one);
			moves.emplace_back(Move(index_pawn, index_move, piece));

			uint64_t push_two = double_push & ~all;
			if (push_two) {
				index_move = get_lsb(&push_two);
				moves.emplace_back(Move(index_pawn, index_move, piece,NO_PIECE,FLAG_PAWN_DOUBLE_PUSH));
			}
		}
		
		//==========================bicie enPassant======================================
		if (pos.enPassantSquare != NO_SQUARE){
			int ep_index = pos.enPassantSquare;
			uint64_t ep_target_bb = 1ULL << ep_index;

			uint64_t ep_capture_targets = pawnAttacks[index_table][index_pawn];

			if (ep_capture_targets & ep_target_bb){
				uint8_t captured_pawn = pos.isWhiteMove ? BLACK_PAWN : WHITE_PAWN; //make_move to te¿ sprawdza
				moves.emplace_back(Move(index_pawn, ep_index, piece, captured_pawn, FLAG_EN_PASSANT));
			}
		}
	}

}




//====================================HETMAN=============================================================
uint64_t get_queen_attacks(int square, uint64_t board) {
	return get_bishop_attacks(square, board) | get_rook_attacks(square, board);
}
void generateQueenMoves(const Position& pos, std::vector<Move>& moves) {
	uint64_t all = pos.getAllPieces();
	uint64_t friendly = pos.getAllFriendlyPieces();

	uint8_t piece = (pos.isWhiteMove) ? WHITE_QUEEN : BLACK_QUEEN;
	uint64_t quenns = pos.bitBoard[piece];

	while (quenns) { //liczy dla danego goñca
		int index_queen = pop_lsb(&quenns);//okreœla jego pole i usuwa jest puli

		uint64_t moves_to = get_queen_attacks(index_queen, all);
		moves_to &= (~friendly); //odejmuje pozycje gdzie s¹ friendly figury

		while (moves_to) {
			int index_move = pop_lsb(&moves_to);

			uint8_t captured = pos.piece_on_square(index_move);
			moves.emplace_back(Move(index_queen, index_move, piece, captured));
		}
	}
}

//===================================================================================================

void initAttackTables()
{
	initPawnTables();
	initKingAttacks();
	initKnightAttacks();
	initBishopMagics();
	initRookMagics();
}
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


