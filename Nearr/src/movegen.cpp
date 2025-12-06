#include "movegen.hpp"


//====================================FUNKCJE BITOWE=============================================================
inline int pop_lsb(uint64_t* bitboard)
{
	unsigned long int index;
	if (_BitScanForward64(&index, *bitboard)){
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

	//co pod jest pod nim
	return RookAttackTable[final_index];
}
void generateRookMoves(const Position& pos, std::vector<Move>& moves)
{

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
void initBishopMagics(){};
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
void generateBishopMoves(const Position& pos, std::vector<Move>& moves) {};

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
void generateKnightMoves(const Position& pos, std::vector<Move>& moves)
{
	uint64_t friendly = pos.bitBoard[pos.isWhiteMove ? WHITE_ALL : BLACK_ALL];
	uint64_t enemy = pos.bitBoard[pos.isWhiteMove ? BLACK_ALL : WHITE_ALL];
	uint64_t knight = pos.bitBoard[pos.isWhiteMove ? WHITE_KNIGHT : BLACK_KNIGHT];


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
}//wypisuje tablice legalnych ruchów dla króla w wybranej jego pozycji
void generateKingMoves(const Position& pos, std::vector<Move>& moves)
{
}

//====================================PION=============================================================
void generatePawnMoves(const Position& pos, std::vector<Move>& moves)
{
}


//====================================HETMAN=============================================================
void generateQueenMoves(const Position& pos, std::vector<Move>& moves)
{
}




void initAttackTables()
{
	initKingAttacks();
	initKnightAttacks();
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


