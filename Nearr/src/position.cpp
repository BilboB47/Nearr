#include "position.hpp"




//===============================USTAWIANIE POZYCJI==================================================
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
void Position::set_position(std::string s, bool isWhite)
{
    isWhiteMove = isWhite;
    memset(bitBoard, 0, sizeof(bitBoard));
    int start_row = 56;
    int column = 0;
    for (int p = 0; p < s.length(); p++){
        if (s[p] == '/'){
            start_row -= 8;
            column = 0;
            continue;
        }

        uint8_t piece = piece_char_to_number(s[p]);
        if (piece != NO_PIECE) {
            bitBoard[piece] |= (1ULL << (start_row + column));
            column++;
        }else {
            column += s[p] - '0';
        }
    }

    bitBoard[12] = bitBoard[0] | bitBoard[1] | bitBoard[2] | bitBoard[3] | bitBoard[4] | bitBoard[5];
    bitBoard[13] = bitBoard[6] | bitBoard[7] | bitBoard[8] | bitBoard[9] | bitBoard[10] | bitBoard[11];
    //rnbqkbnr / pppppppp / 8 / 8 / 8 / 8 / PPPPPPPP / RNBQKBNR
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
        bitBoard[(isWhiteMove) ? BLACK_ALL : WHITE_ALL] &= ~(1ULL << move.to); //usuwa z koloru zbitego
    }

    //promocja pionka na dame
    

    //uint8_t castlingRights;
    //uint8_t enPassantSquare; 
    

    //uint8_t promotion;

    isWhiteMove = !isWhiteMove;
}




//===================ZBIERANIE INFORMACJI NA TEMAT PLANSZY==============================================================
uint64_t Position::getAllPieces() const{
    return bitBoard[WHITE_ALL] | bitBoard[BLACK_ALL];
}
uint64_t Position::getAllFriendlyPieces() const {
    return (isWhiteMove) ? bitBoard[WHITE_ALL] : bitBoard[BLACK_ALL];
}
uint64_t Position::getAllEnemyPieces() const {
    return (isWhiteMove) ? bitBoard[BLACK_ALL] : bitBoard[WHITE_ALL];
}
uint8_t Position::piece_on_square(int sq) const
{   
    if ((getAllPieces() & (1ULL << sq)) == (0ULL)) return NO_PIECE;

    for (int i = 0; i < 12; i++) {
        if (bitBoard[i] & (1ULL << sq))return i;//i - to index danej figury 
    }
    return NO_PIECE;
}

uint8_t Position::piece_char_to_number(const char c)
{
    switch (c)
    {
    case 'P': return WHITE_PAWN;
    case 'N': return WHITE_KNIGHT;
    case 'B': return WHITE_BISHOP;
    case 'R': return WHITE_ROOK;
    case 'Q': return WHITE_QUEEN;
    case 'K': return WHITE_KING;

    case 'p': return BLACK_PAWN;
    case 'n': return BLACK_KNIGHT;
    case 'b': return BLACK_BISHOP;
    case 'r': return BLACK_ROOK;
    case 'q': return BLACK_QUEEN;
    case 'k': return BLACK_KING;

    default:
        return NO_PIECE;
    }
}


