#include "position.hpp"

void Position::set_start_position() {
    board[0] = 0x000000000000FF00ULL; // white pawns
    board[1] = 0x0000000000000042ULL; // white knights
    board[2] = 0x0000000000000024ULL; // white bishops
    board[3] = 0x0000000000000081ULL; // white rooks
    board[4] = 0x0000000000000008ULL; // white queen
    board[5] = 0x0000000000000010ULL; // white king

    board[6] = 0x00FF000000000000ULL; // black pawns
    board[7] = 0x4200000000000000ULL; // black knights
    board[8] = 0x2400000000000000ULL; // black bishops
    board[9] = 0x8100000000000000ULL; // black rooks
    board[10] = 0x0800000000000000ULL; // black queen
    board[11] = 0x1000000000000000ULL; // black king

    // occupancy
    board[12] = board[0] | board[1] | board[2] | board[3] | board[4] | board[5];
    board[13] = board[6] | board[7] | board[8] | board[9] | board[10] | board[11];

    isWhiteMove = true;
    castlingRights = 0b1111;  // wszystkie roszady dozwolone
    enPassantSquare = 0;      // brak
    moveNumber = 1;           // pierwszy ruch
}

void Position::make_move(const Move& move){

    if (move.piece == NO_PIECE)return;

    //usun i dodaj ruszon¹ figure
    board[move.piece] &= ~(1ULL << move.from);
    board[move.piece] |= (1ULL << move.to);
    
    //usun zbit¹ figure
    if (move.captured != NO_PIECE){
        board[move.captured] &= ~(1ULL << move.to);
    }

    //uint8_t castlingRights;
    //uint8_t enPassantSquare;
    

    //uint8_t promotion;

    isWhiteMove = !isWhiteMove;
}

