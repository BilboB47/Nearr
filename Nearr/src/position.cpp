#include "position.hpp"

void Position::set_start_position() {
    bitBoard[0] = 0x000000000000FF00ULL; // white pawns
    bitBoard[1] = 0x0000000000000042ULL; // white knights
    bitBoard[2] = 0x0000000000000024ULL; // white bishops
    bitBoard[3] = 0x0000000000000081ULL; // white rooks
    bitBoard[4] = 0x0000000000000008ULL; // white queen
    bitBoard[5] = 0x0000000000000010ULL; // white king

    bitBoard[6] = 0x00FF000000000000ULL; // black pawns
    bitBoard[7] = 0x4200000000000000ULL; // black knights
    bitBoard[8] = 0x2400000000000000ULL; // black bishops
    bitBoard[9] = 0x8100000000000000ULL; // black rooks
    bitBoard[10] = 0x0800000000000000ULL; // black queen
    bitBoard[11] = 0x1000000000000000ULL; // black king

    // occupancy
    bitBoard[12] = bitBoard[0] | bitBoard[1] | bitBoard[2] | bitBoard[3] | bitBoard[4] | bitBoard[5];
    bitBoard[13] = bitBoard[6] | bitBoard[7] | bitBoard[8] | bitBoard[9] | bitBoard[10] | bitBoard[11];

    isWhiteMove = true;
    castlingRights = 0b1111;  // wszystkie roszady dozwolone
    enPassantSquare = 0;      // brak
    moveNumber = 1;           // pierwszy ruch
}

void Position::make_move(const Move& move){

    if (move.piece == NO_PIECE)return;

    //usun i dodaj ruszon¹ figure
    bitBoard[move.piece] &= ~(1ULL << move.from);
    bitBoard[move.piece] |= (1ULL << move.to);

    //usun i dodaj z biboard gdzie s¹ wszystkie figury
    bitBoard[(isWhiteMove) ? WHITE_ALL : BLACK_ALL] &= ~(1ULL << move.from);
    bitBoard[(isWhiteMove) ? WHITE_ALL : BLACK_ALL] |= (1ULL << move.to);

    //usun zbit¹ figure z bitboard koloru i bierki
    if (move.captured != NO_PIECE){
        bitBoard[move.captured] &= ~(1ULL << move.to);
        bitBoard[(isWhiteMove) ? BLACK_ALL : WHITE_ALL] |= (1ULL << move.to); //usuwa z koloru zbitego
    }

    //promocja pionka na dame
    
    //uint8_t castlingRights;
    //uint8_t enPassantSquare; 
    

    //uint8_t promotion;

    isWhiteMove = !isWhiteMove;
}

uint8_t Position::piece_on_square(int sq) const
{
    for (int i = 0; i < 12; i++) {
        if (bitBoard[i] & (1ULL << sq))return i;//i - to index danej figury 
    }
    return NO_PIECE;
}

