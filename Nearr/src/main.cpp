#include <iostream>
#include "ui_console.hpp"
#include "position.hpp"
#include "movegen.hpp"

#include <string>

std::string squareToString(int index) {
	int file = index % 8;  // kolumna (A–H)
	int rank = index / 8;  // rząd (1–8)
	char fileChar = 'A' + file;
	char rankChar = '1' + rank;
	return std::string() + fileChar + rankChar;
}

void test_print(Position& p) {
    vector<Move> moves;
    generatePawnMoves(p, moves);

    char board[8][8];
    ToArray(p, board);
    printBoard(board);

    cout << "Liczba ruchów pionów: " << moves.size() << "\n\n";
    for (auto& m : moves) {
        cout << "from: " << int(m.from)
            << " to: " << int(m.to)
            << " piece: " << int(m.piece)
            << " captured: " << int(m.captured)
            << " promo: " << int(m.promotion)
            << "\n";
    }
    cout << "\n---------------------------------\n\n";
}


// ------------------------------------------------------
// SCENARIUSZ 1 — pion bez blokad, zwykłe ruchy
// ------------------------------------------------------
void scenario1() {
    Position p;
    memset(p.bitBoard, 0, sizeof(p.bitBoard));
    p.isWhiteMove = true;
    p.enPassantSquare = 255;

    // pion na e2 = pole 12
    p.bitBoard[WHITE_PAWN] |= 1ULL << 12;
    p.bitBoard[WHITE_ALL] |= 1ULL << 12;

    cout << "SCENARIUSZ 1 — zwykłe ruchy bez blokad\n";
    test_print(p);
}


// ------------------------------------------------------
// SCENARIUSZ 2 — pion zablokowany
// ------------------------------------------------------
void scenario2() {
    Position p;
    memset(p.bitBoard, 0, sizeof(p.bitBoard));
    p.isWhiteMove = true;
    p.enPassantSquare = 255;

    // biały pion na e2
    p.bitBoard[WHITE_PAWN] |= 1ULL << 12;
    p.bitBoard[WHITE_ALL] |= 1ULL << 12;

    // czarny pion na e3 (blok)
    p.bitBoard[BLACK_PAWN] |= 1ULL << 20;
    p.bitBoard[BLACK_ALL] |= 1ULL << 20;

    cout << "SCENARIUSZ 2 — pion zablokowany\n";
    test_print(p);
}


// ------------------------------------------------------
// SCENARIUSZ 3 — bicie w lewo i prawo
// ------------------------------------------------------
void scenario3() {
    Position p;
    memset(p.bitBoard, 0, sizeof(p.bitBoard));
    p.isWhiteMove = true;
    p.enPassantSquare = 255;

    // biały pion na d4 (pole 27)
    p.bitBoard[WHITE_PAWN] |= 1ULL << 27;
    p.bitBoard[WHITE_ALL] |= 1ULL << 27;

    // czarne piony na c5 (34) i e5 (36)
    p.bitBoard[BLACK_PAWN] |= 1ULL << 34;
    p.bitBoard[BLACK_ALL] |= 1ULL << 34;

    p.bitBoard[BLACK_PAWN] |= 1ULL << 36;
    p.bitBoard[BLACK_ALL] |= 1ULL << 36;

    cout << "SCENARIUSZ 3 — bicie w lewo i prawo\n";
    test_print(p);
}


// ------------------------------------------------------
// SCENARIUSZ 4 — promocja pionka
// ------------------------------------------------------
void scenario4() {
    Position p;
    memset(p.bitBoard, 0, sizeof(p.bitBoard));
    p.isWhiteMove = true;
    p.enPassantSquare = 255;

    // biały pion na a7 (pole 48)
    p.bitBoard[WHITE_PAWN] |= 1ULL << 48;
    p.bitBoard[WHITE_ALL] |= 1ULL << 48;

    cout << "SCENARIUSZ 4 — promocja pionka\n";
    test_print(p);
}



int main() {
     scenario4();
}
