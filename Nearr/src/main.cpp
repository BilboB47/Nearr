#include <iostream>
#include "ui_console.hpp"
#include "position.hpp"
#include "movegen.hpp"

#include <chrono>
#include <iomanip>

using namespace std::chrono;


void test_en_passant_move() {
    Position pos;

    // Pozycja FEN: Biały pionek na e5, Czarny pionek na d5, EP Square to d6 (43)
    std::string fen_ep_ready = "rnbqkbnr/ppp1pppp/8/3pP3/3P4/8/PPP2PPP/RNBQKBNR w KQkq d6 0 3";

    // 1. Inicjalizacja pozycji
    pos.set_position_FEN(fen_ep_ready);

    // Weryfikacja: Pole En Passant powinno być d6 (indeks 43)
    // Jeśli wcześniej występował błąd 0 lub 204, ten test się nie powiedzie, 
    // dopóki nie poprawisz przesunięcia 'i' w set_position_FEN.
    const int EXPECTED_EP_SQUARE = 43;
    if (pos.enPassantSquare != EXPECTED_EP_SQUARE) {
        std::cout << "--- OSTRZEŻENIE ---" << std::endl;
        std::cout << "BLAD W PARSOWANIU: Pole EP jest niepoprawne. Oczekiwano " << EXPECTED_EP_SQUARE
            << ", otrzymano " << (int)pos.enPassantSquare << std::endl;
        std::cout << "--- Test make_move MOŻE się nie powieść ---" << std::endl;
    }
    else {
        std::cout << "TEST 1: Inicjalizacja EP OK (Pole: d6 / 43)." << std::endl;
    }


    // 2. Definicja ruchu En Passant (Biały e5 -> d6, bije czarnego pionka na d5)
    Move ep_move;
    ep_move.from = 36; // e5
    ep_move.to = 43;   // d6 (pole en passant)
    ep_move.piece = WHITE_PAWN;
    ep_move.captured = NO_PIECE; // W ruchu EP captured jest NO_PIECE
    ep_move.flags = FLAG_EN_PASSANT;

    std::cout << "\n--- Plansza PRZED ruchem EP ---\n";
    pos.print_board();

    // 3. Wykonanie ruchu
    pos.make_move(ep_move);

    // 4. Weryfikacja po ruchu
    std::cout << "\n--- Plansza PO ruchu EP (e5xd6) ---\n";
    pos.print_board();

    uint64_t target_d6_bb = 1ULL << 43;  // d6 (miejsce, gdzie ma wylądować biały pionek)
    uint64_t captured_d5_bb = 1ULL << 35; // d5 (miejsce, z którego czarny pionek ma zniknąć)

    // Sprawdzamy, czy pionki są tam, gdzie powinny
    bool white_pawn_moved = (pos.bitBoard[WHITE_PAWN] & target_d6_bb) != 0;
    bool black_pawn_removed = (pos.bitBoard[BLACK_PAWN] & captured_d5_bb) == 0;

    // Sprawdzamy stan EP Square i tury
    bool ep_cleared = (pos.enPassantSquare == NO_SQUARE);
    bool turn_changed = (!pos.isWhiteMove); // Tura powinna zmienić się na Czarnych

    // Sprawdzenie finalne
    if (white_pawn_moved && black_pawn_removed && ep_cleared && turn_changed) {
        std::cout << "\n✅ TEST SUKCES: Ruch En Passant dziala poprawnie!" << std::endl;
    }
    else {
        std::cout << "\n❌ TEST BLAD:" << std::endl;
        if (!white_pawn_moved) std::cout << " - Blad: BIALY pionek nie znalazl sie na polu d6 (43)." << std::endl;
        if (!black_pawn_removed) std::cout << " - Blad: CZARNY pionek na polu d5 (35) NIE zostal usuniety." << endl;
            if (!ep_cleared) std::cout << " - Blad: Pole EP Square (enPassantSquare) nie zostalo wyczyszczone." << std::endl;
        if (!turn_changed) std::cout << " - Blad: Tura gracza nie zostala zmieniona." << std::endl;
    }
}
void test_promotion() {
    Position pos;
    // FEN z pionkiem na f7, gotowym do promocji
    // W FEN brakuje wież na a1 i h1, ale to nie wpływa na test promocji
    std::string fen_promo = "8/5P2/8/8/8/8/8/RNBQKBNR w KQ - 0 1";

    pos.set_position_FEN(fen_promo);
    std::cout << "\n--- TEST PROMOCJI (f7 -> f8 na Hetmana) ---\n";


    // 1. Definicja ruchu promocji (f7 -> f8 na Hetmana)
    Move promo_move;
    promo_move.from = 53; // f7
    promo_move.to = 61;   // f8
    promo_move.piece = WHITE_PAWN;
    promo_move.captured = NO_PIECE;
    //promo_move.flags = FLAG_PROMOTION;
    promo_move.promotion = PROMOTION_QUEEN; 

    std::cout << "Plansza PRZED promocją:\n";
    pos.print_board();

    // 2. Wykonanie ruchu
    pos.make_move(promo_move);

    // 3. Weryfikacja po ruchu
    std::cout << "Plansza PO promocji:\n";
    pos.print_board();


    // Oczekiwane stany Bitboards
    uint64_t f7_bb = 1ULL << 53;
    uint64_t f8_bb = 1ULL << 61;

    bool pawn_removed = (pos.bitBoard[WHITE_PAWN] & f7_bb) == 0;
    bool queen_added = (pos.bitBoard[WHITE_QUEEN] & f8_bb) != 0;
    bool turn_changed = (!pos.isWhiteMove); // Sprawdzenie, czy tura się zmieniła na czarne

    if (pawn_removed && queen_added && turn_changed) {
        std::cout << "✅ TEST PROMOCJI SUKCES: Pionek zamieniony na Hetmana na f8.\n";
    }
    else {
        std::cout << "❌ TEST PROMOCJI BLAD:\n";
        if (!pawn_removed) std::cout << " - Blad: Pionek (" << (pos.bitBoard[WHITE_PAWN] & f7_bb) << ") nie zostal usuniety z f7.\n";
        if (!queen_added) std::cout << " - Blad: Hetman (" << (pos.bitBoard[WHITE_QUEEN] & f8_bb) << ") nie pojawil sie na f8.\n";
        if (!turn_changed) std::cout << " - Blad: Tura gracza nie zostala zmieniona.\n";
    }
}
void test_castling_kingside() {
    Position pos;

    // FEN: Pozycja z wolnymi polami f1 i g1, Król i Wieża w pozycji wyjściowej, z prawami roszady 'K'.
    // W FEN używamy tylko R i K, aby ułatwić weryfikację. Prawa Roszady 'K' są w sekcji 4.
    std::string fen_castling = "8/8/8/8/8/8/8/4K2R w K - 0 1";

    pos.set_position_FEN(fen_castling);
    std::cout << "\n--- TEST ROSZADY KRÓLEWSKIEJ (O-O, e1 -> g1) ---\n";

    // Upewniamy się, że prawa roszady są ustawione
    if (!(pos.castlingRights & WK)) {
        std::cout << "❌ BLAD INICJALIZACJI: Brak praw roszady (WK) wczytanych z FEN.\n";
        return;
    }

    // 1. Definicja ruchu Roszady (Król e1 -> g1)
    Move castling_move;
    castling_move.from = 4; // e1 (start Króla)
    castling_move.to = 6;   // g1 (koniec Króla)
    castling_move.piece = WHITE_KING;
    castling_move.captured = NO_PIECE;
    castling_move.flags = FLAG_CASTLE_KINGSIDE; // Używamy Twojej flagi O-O
    castling_move.promotion = 0; // Bez promocji

    std::cout << "Plansza PRZED Roszadą:\n";
    pos.print_board();

    // 2. Wykonanie ruchu
    pos.make_move(castling_move);

    // 3. Weryfikacja po ruchu
    std::cout << "Plansza PO Roszadzie:\n";
    pos.print_board();

        // Oczekiwane stany pól:
        uint64_t king_e1 = 1ULL << 4; // e1
    uint64_t king_g1 = 1ULL << 6; // g1
    uint64_t rook_h1 = 1ULL << 7; // h1
    uint64_t rook_f1 = 1ULL << 5; // f1

    // A. Czy Król się przesunął? (e1 -> g1)
    bool king_moved = (pos.bitBoard[WHITE_KING] & king_e1) == 0 &&
        (pos.bitBoard[WHITE_KING] & king_g1) != 0;
    // B. Czy Wieża się przesunęła? (h1 -> f1)
    bool rook_moved = (pos.bitBoard[WHITE_ROOK] & rook_h1) == 0 &&
        (pos.bitBoard[WHITE_ROOK] & rook_f1) != 0;
    // C. Czy usunięto prawa roszady? (WK musi zniknąć)
    bool rights_updated = (pos.castlingRights & WK) == 0;
    // D. Czy tura się zmieniła?
    bool turn_changed = (!pos.isWhiteMove);

    if (king_moved && rook_moved && rights_updated && turn_changed) {
        std::cout << "✅ TEST ROSZADY SUKCES: O-O wykonane poprawnie.\n";
    }
    else {
        std::cout << "❌ TEST ROSZADY BLAD:\n";
        if (!king_moved) std::cout << " - Blad A: Król nie przesunal sie z e1 na g1.\n";
        if (!rook_moved) std::cout << " - Blad B: Wieża nie przesunela sie z h1 na f1.\n";
        if (!rights_updated) std::cout << " - Blad C: Prawa Roszady (WK) nie zostaly usunięte.\n";
        if (!turn_changed) std::cout << " - Blad D: Tura gracza nie zostala zmieniona.\n";
    }
}
void test_lose_rights_king_move() {
    Position pos;
    // FEN z Królem na e1 i wszystkimi prawami roszady 'KQkq'
    std::string fen = "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1";
    pos.set_position_FEN(fen);

    Move king_move;
    king_move.from = 4; // e1
    king_move.to = 3;   // d1
    king_move.piece = WHITE_KING;
    king_move.flags = FLAG_NONE; // Zwykły ruch

    pos.make_move(king_move);

    bool wk_lost = (pos.castlingRights & WK) == 0;
    bool wq_lost = (pos.castlingRights & WQ) == 0;
    bool bk_kept = (pos.castlingRights & BK) != 0; // Prawa Czarnych zachowane

    if (wk_lost && wq_lost && bk_kept) {
        std::cout << "✅ TEST UTRATY PRAW KRÓLA SUKCES. Nowe prawa: " << (int)pos.castlingRights << std::endl;
    }
    else {
        std::cout << "❌ TEST UTRATY PRAW KRÓLA BLAD." << std::endl;
    }
}
void test_lose_rights_capture_rook() {
    Position pos;
    // FEN: Czarny Skoczek na b3 (18), Biała Wieża na a1 (0). Ruch Czarnych. Prawa 'KQkq'.
    std::string fen = "r3k2r/8/8/8/8/1n6/8/R3K2R b KQkq - 0 1";
    pos.set_position_FEN(fen);

    Move capture_move;
    capture_move.from = 18; // b3 (Czarny Skoczek)
    capture_move.to = 0;   // a1 (Biała Wieża)
    capture_move.piece = BLACK_KNIGHT;
    capture_move.captured = WHITE_ROOK; // Bicie
    capture_move.flags = FLAG_NONE;

    pos.make_move(capture_move);

    // Biały traci prawo Q, bo zbita została jego Wieża z a1
    bool wq_lost = (pos.castlingRights & WQ) == 0;
    // Białe prawo K i Czarne prawa muszą być zachowane
    bool wk_kept = (pos.castlingRights & WK) != 0;

    if (wq_lost && wk_kept) {
        std::cout << "✅ TEST UTRATY PRAW BICIE WIEŻY SUKCES. Nowe prawa: " << (int)pos.castlingRights << std::endl;
    }
    else {
        std::cout << "❌ TEST UTRATY PRAW BICIE WIEŻY BLAD." << std::endl;
    }
}

int main() {
	//initKingAttacks();
	//initKnightAttacks();

	//Position pozycja = Position();
	//pozycja.set_position("r3qrk1/4bppp/p1N1pn2/1p6/5B2/2P5/PP3PPP/R2QR1K1",true);
	////pozycja.set_start_position();  

	//char board[8][8];
	//ToArray(pozycja, board);
	//printBoard(board);
	//cout << endl;

    //initAttackTables();
    
    //test_en_passant_move();
    //test_promotion();
    //test_castling_kingside();
    //test_lose_rights_capture_rook();
    //test_lose_rights_king_move();

    return 0;

}
