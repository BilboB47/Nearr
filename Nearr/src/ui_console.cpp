#include "ui_console.hpp"

void ToArray(const Position& curP, char(&board)[8][8]) {

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            board[i][j] = ((i + j) % 2 == 0) ? '.' : '#';
        }
    }


    for (int i = 0; i < 12; i++) {
        char piece = ' ';
        switch (i) {
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
        default: piece = '.'; break;
        }

        for (int j = 0; j < 64; j++) {
            if (curP.bitBoard[i] & (1ULL << j)) {
                int row = 7 - (j / 8); 
                int col = j % 8;
                board[row][col] = piece;
            }
        }
    }

}

wchar_t pieceToUnicode(char p) {
    switch (p) {
    case 'r': return L'♜';
    case 'n': return L'♞';
    case 'b': return L'♝';
    case 'q': return L'♛';
    case 'k': return L'♚';
    case 'p': return L'♟';
    case 'R': return L'♖';
    case 'N': return L'♘';
    case 'B': return L'♗';
    case 'Q': return L'♕';
    case 'K': return L'♔';
    case 'P': return L'♙';
    default:  return L' ';
    }
    return L' ';
}

void printBoard(char(&board)[8][8]){
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            cout << board[i][j];
        }cout << "\n";
    }
}

void printBoard(wchar_t(&board)[8][8]){
}

