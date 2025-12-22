#include "ui_console.hpp"


char get_piece_from_number(uint8_t n) {
    char piece = ' ';
    switch (n) {
    case 0:  piece = 'P'; break; // white pawns
    case 1:  piece = 'N'; break;
    case 2:  piece = 'B'; break;
    case 3:  piece = 'R'; break;
    case 4:  piece = 'Q'; break;
    case 5:  piece = 'K'; break;
    case 6:  piece = 'p'; break; // black pawns
    case 7:  piece = 'n'; break;
    case 8:  piece = 'b'; break;
    case 9:  piece = 'r'; break;
    case 10: piece = 'q'; break;
    case 11: piece = 'k'; break;
    default: piece = ' '; break;
    }
    return piece;
}

std::string squareToString(int index) {
    int file = index % 8;  // kolumna (A–H)
    int rank = index / 8;  // rząd (1–8)
    char fileChar = 'A' + file;
    char rankChar = '1' + rank;
    return std::string() + fileChar + rankChar;
}

void print_move(const Move m) {
    std::string fromSquare = squareToString(m.from);
    std::string toSquare = squareToString(m.to);

    std::cout << fromSquare << toSquare;

    std::cout << std::endl;
}

