#include "position.hpp"




//===============================USTAWIANIE POZYCJI==================================================
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
void Position::set_position_FEN(std::string s) {

    memset(bitBoard, 0, sizeof(bitBoard));

    int i = 0;
    int row = 7;

    //figury
    while (i < s.length() && s[i] != ' ') {
        int col = 0;
        while (i < s.length() && s[i] != '/' && s[i] != ' ') {
            if (col >= 8) {
                break;
            }

            if (s[i] >= '1' && s[i] <= '8') {
                col += s[i] - '0';
            }
            else {
                uint8_t piec = piece_char_to_number(s[i]);
                
                this->bitBoard[piec] |= (1ULL) << (row * 8 + col);
                col++;
            }
            i++;
        }
        if (i >= s.length() || s[i] == ' ') break;
        i++;
        row--;
    }

    i++; 
    this->isWhiteMove = (s[i] == 'w');

    //prawa do roszady
    this->castlingRights = 0;
    i++; // Pominiêcie 'w'/'b'
    i++; // Pominiêcie spacji 

    while (s[i] != ' ') {
        if (s[i] == 'K')this->castlingRights |= WK;
        else if (s[i] == 'Q')this->castlingRights |= WQ;
        else if (s[i] == 'k')this->castlingRights |= BK;
        else if (s[i] == 'q')this->castlingRights |= BQ;
        i++;
    }
    i++; // Pominiêcie spacji

    //EnPassant 
    int index = NO_SQUARE;

    if (s[i] == '-') {
        i++; // skip jesli -
    }
    else {
        int col = s[i] - 'a';
        int rank_index = (s[i + 1] - '0') - 1;
        index = (rank_index * 8) + col;
        i += 2; //idze na spacje
    }
    this->enPassantSquare = index;

    //Half move
    i++; // Pominiêcie spacji po En Passant
    std::string number = "";
    while (s[i] != ' ') {
        number += s[i];
        i++;
    }
    this->halfmoveClock = stoi(number);

    // 6. Iloœæ ruchów
    i++; // Pominiêcie spacji po Half move
    number = "";
    while (i < s.length()) {
        number += s[i];
        i++;
    }
    this->moveNumber = stoi(number);

    //update all
    this->bitBoard[WHITE_ALL] = this->bitBoard[WHITE_PAWN] |
        this->bitBoard[WHITE_KNIGHT] |
        this->bitBoard[WHITE_BISHOP] |
        this->bitBoard[WHITE_ROOK] |
        this->bitBoard[WHITE_QUEEN] |
        this->bitBoard[WHITE_KING];

    this->bitBoard[BLACK_ALL] = this->bitBoard[BLACK_PAWN] |
        this->bitBoard[BLACK_KNIGHT] |
        this->bitBoard[BLACK_BISHOP] |
        this->bitBoard[BLACK_ROOK] |
        this->bitBoard[BLACK_QUEEN] |
        this->bitBoard[BLACK_KING];
}


//========================================ROBIENIE RUCHU===============================================================
void Position::make_simple_move(uint8_t piece_type, uint64_t from_bb, uint64_t to_bb, uint8_t moving_color_all)
{
    // usun figure z starego pola
    this->bitBoard[piece_type] &= ~from_bb; // U¿ywamy piece_type (Wie¿a lub Król)
    this->bitBoard[moving_color_all] &= ~from_bb;

    // dodaj figure na nowe pole
    this->bitBoard[piece_type] |= to_bb; // U¿ywamy piece_type (Wie¿a lub Król)
    this->bitBoard[moving_color_all] |= to_bb;
}
void Position::remove_captured_piece(uint8_t captured_piece, uint64_t captured_square_bb, uint8_t captured_color_all) {
    this->bitBoard[captured_piece] &= ~captured_square_bb;
    this->bitBoard[captured_color_all] &= ~captured_square_bb;
}
void Position::promote_pawn(const Move& move, uint64_t to_bb)
{
    uint8_t promotion_to;

    if (move.promotion & PROMOTION_QUEEN) {
        promotion_to = (isWhiteMove) ? WHITE_QUEEN : BLACK_QUEEN;
    }
    else if (move.promotion & PROMOTION_KNIGHT) {
        promotion_to = (isWhiteMove) ? WHITE_KNIGHT : BLACK_KNIGHT;
    }
    else if (move.promotion & PROMOTION_ROOK) {
        promotion_to = (isWhiteMove) ? WHITE_ROOK : BLACK_ROOK;
    }
    else if (move.promotion & PROMOTION_BISHOP) {
        promotion_to = (isWhiteMove) ? WHITE_BISHOP : BLACK_BISHOP;
    }
    else {
        throw("MOVE WRONG PROMOTION");
    }

    this->bitBoard[move.piece] &= ~to_bb; //usun pion ktory jest promowany
    this->bitBoard[promotion_to] |= to_bb; //dodaj now¹ figure
}
void Position::update_castling_rights(const Move& move)
{
    //ruch krola
    if (move.piece == WHITE_KING) this->castlingRights &= ~(WK | WQ);
    if (move.piece == BLACK_KING) this->castlingRights &= ~(BK | BQ);

    //ruch wie¿y
    if (move.piece == WHITE_ROOK) {
        if (move.from == A1) this->castlingRights &= ~WQ; 
        if (move.from == H1) this->castlingRights &= ~WK; 
    }
    if (move.piece == BLACK_ROOK) {
        if (move.from == A8) this->castlingRights &= ~BQ; 
        if (move.from == H8) this->castlingRights &= ~BK; 
    }

    //zbicie wiezy
    if (move.captured == WHITE_ROOK) {
        if (move.to == A1) this->castlingRights &= ~WQ; 
        if (move.to == H1) this->castlingRights &= ~WK; 
    }
    if (move.captured == BLACK_ROOK) {
        if (move.to == A8) this->castlingRights &= ~BQ; 
        if (move.to == H8) this->castlingRights &= ~BK; 
    }
}
void Position::handle_castling_rook(const Move& move) {
    Square rook_from_sq, rook_to_sq;
    uint8_t rook_piece = isWhiteMove ? WHITE_ROOK : BLACK_ROOK;

    if (move.flags & FLAG_CASTLE_KINGSIDE) {
        if (isWhiteMove) {
            rook_from_sq = H1;
            rook_to_sq = F1;
        }
        else {
            rook_from_sq = H8;
            rook_to_sq = F8;
        }
    }
    else if (move.flags & FLAG_CASTLE_QUEENSIDE) {
        if (isWhiteMove) {
            rook_from_sq = A1;
            rook_to_sq = D1;
        }
        else {
            rook_from_sq = A8;
            rook_to_sq = D8;
        }
    }

    uint64_t from_bb = 1ULL << rook_from_sq;
    uint64_t to_bb = 1ULL << rook_to_sq;
    uint8_t moving_color_all = (isWhiteMove) ? WHITE_ALL : BLACK_ALL;

    make_simple_move(rook_piece, from_bb, to_bb, moving_color_all);
}

//zrobienie ruchu z uwzgledniem wszystkiego (bez sprawdzania leglnoœci ruchu)
void Position::make_move(const Move& move){

        if (move.piece == NO_PIECE)return;

        uint64_t from_bb = 1ULL << move.from;
        uint64_t to_bb = 1ULL << move.to;

        Piece moving_color_all = (isWhiteMove) ? WHITE_ALL : BLACK_ALL;
        Piece captured_color_all = (isWhiteMove) ? BLACK_ALL : WHITE_ALL;

        this->enPassantSquare = NO_SQUARE;

        //bicie figury
        if (move.captured != NO_PIECE && !(move.flags & FLAG_EN_PASSANT)){
            remove_captured_piece(move.captured, to_bb, captured_color_all);
        }
        make_simple_move(move.piece, from_bb,to_bb,moving_color_all);

        //promocja piona
        if (move.flags & FLAG_PROMOTION){
            promote_pawn(move, to_bb);
        }

        //ustawienie pol enPassant
        if (move.flags & FLAG_PAWN_DOUBLE_PUSH) {
            if (isWhiteMove){
                enPassantSquare = move.from+8;
            }else {
                enPassantSquare = move.from-8;
            }
        }
    
        //bicie en Passant
        if (move.flags & FLAG_EN_PASSANT) {
            //w zale¿nosci od koloru bierze pole te ktorê przeskoczy³ pion ktory poruszyl sie o 2
            uint64_t captured_square_bb = (isWhiteMove) ? to_bb >> 8 : to_bb << 8;
            uint8_t captured_pawn = (isWhiteMove) ? BLACK_PAWN: WHITE_PAWN;
            
            remove_captured_piece(captured_pawn, captured_square_bb, captured_color_all);
        }

        //czy nie straci³y sie prawa do roszady
        update_castling_rights(move);

        //wykonanie roszady
        if (move.flags&(FLAG_CASTLE_KINGSIDE|FLAG_CASTLE_QUEENSIDE)){
            handle_castling_rook(move);
        }

        if (move.captured != NO_PIECE || move.piece == WHITE_PAWN || move.piece == BLACK_PAWN) {
            this->halfmoveClock = 0;
        }else {
            this->halfmoveClock++;
        }

        if (!this->isWhiteMove)this->moveNumber++;
        
        this->isWhiteMove = !this->isWhiteMove;
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

char Position::piece_to_char(int piece_index) {
    // Prosta konwersja indeksu piece_index (0-11) na symbol FEN
    switch (piece_index) {
    case WHITE_PAWN: return 'P';
    case WHITE_KNIGHT: return 'N';
    case WHITE_BISHOP: return 'B';
    case WHITE_ROOK: return 'R';
    case WHITE_QUEEN: return 'Q';
    case WHITE_KING: return 'K';
    case BLACK_PAWN: return 'p';
    case BLACK_KNIGHT: return 'n';
    case BLACK_BISHOP: return 'b';
    case BLACK_ROOK: return 'r';
    case BLACK_QUEEN: return 'q';
    case BLACK_KING: return 'k';
    default: return ' ';
    }
}
void Position::print_board() {
    std::cout << "  ---------------------------------\n";
    for (int rank = 7; rank >= 0; rank--) {
        std::cout << rank + 1 << " |"; // Numer ranki
        for (int file = 0; file <= 7; file++) {
            int square = rank * 8 + file;
            char piece_char = ' ';

            // Szukamy figury na danym polu
            for (int piece_index = WHITE_PAWN; piece_index <= BLACK_KING; piece_index++) {
                if (this->bitBoard[piece_index] & (1ULL << square)) {
                    piece_char = piece_to_char(piece_index);
                    break;
                }
            }
            std::cout << " " << piece_char << " |";
        }
        std::cout << "\n  ---------------------------------\n";
    }
    std::cout << "    a   b   c   d   e   f   g   h\n";
    std::cout << "Aktywny ruch: " << (this->isWhiteMove ? "Bialych (w)" : "Czarnych (b)") << "\n";
    std::cout << "EP Square: " << (this->enPassantSquare == NO_SQUARE ? "-" : std::to_string(this->enPassantSquare)) << "\n";
}

