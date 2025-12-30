
#include "search.hpp"

//=========================SEARCH================================================================
Move Search::get_best_move(Position& pos, int max_depth) {

    Move best_move;
    int best_score = -33000;
    int alpha = -33000;
    int beta = 33000;

    Move moves[256];
    int count = generateMoves(pos, moves);

    //punktacja danego rucha
    int scores[256];
    for (int i = 0; i < count; i++) {
        scores[i] = score_move(pos, moves[i]);
    }

    bool any_legal_move = false; //czy istnieje jakiœ ruch

    //obsluga kazdego ruchu
    for (int i = 0; i < count; i++) {

        //Order move na podstawie jego score
        for (int j = i + 1; j < count; j++) {
            if (scores[j] > scores[i]) {
                std::swap(scores[i], scores[j]);
                std::swap(moves[i], moves[j]);
            }
        }

        UndoInfo info = pos.make_move(moves[i]);

        //jest legalny?
        if (is_in_check_enemy(pos)) {
            pos.unmake_move(moves[i], info);
            continue;
        }

        any_legal_move = true;

        //szukanie w alpha_beta
        int score = -alpha_beta(pos, max_depth - 1, -beta, -alpha, max_depth);

        pos.unmake_move(moves[i], info);

        //zapis najlepszego ruchu
        if (score > best_score) {
            best_score = score;
            best_move = moves[i];
        }

        if (score > alpha) {
            alpha = score;
        }
    }

    //jeœli nie istnieje ¿aden ruch zwróc pusty Move
    if (!any_legal_move) return Move();

    return best_move;
}

int Search::alpha_beta(Position& pos, int depth, int alpha, int beta, int root_depth) {

    // alpha: najlepszy wynik gwarantowany dla gracza (dolna granica)
    // beta:  najlepszy wynik gwarantowany dla przeciwnika (górna granica)

    nodes++;//inkrementuje szukane wez³y

    if (depth <= 0) return evaluate(pos);//zwraca wartoœæ pozycji

    Move moves[256];
    int count = generateMoves(pos, moves);
    int scores[256];

    for (int i = 0; i < count; i++) {
        scores[i] = score_move(pos, moves[i]);
    }

    bool any_legal_move = false;

    for (int i = 0; i < count; i++) {

        for (int j = i + 1; j < count; j++) { //move ordering
            if (scores[j] > scores[i]) {
                std::swap(scores[i], scores[j]);
                std::swap(moves[i], moves[j]);
            }
        }

        UndoInfo info = pos.make_move(moves[i]);//filtr czy jest legalny
        if (is_in_check_enemy(pos)) {
            pos.unmake_move(moves[i], info);
            continue;
        }
        any_legal_move = true;

        //patrzenie glêbiej (zmiana znaku bo jest to subiektywne wzgledem strony dlaczego przewaga -100 dla czarnych to +100 dla bia³ych)
        int score = -alpha_beta(pos, depth - 1, -beta, -alpha, root_depth);

        pos.unmake_move(moves[i], info);

        if (score >= beta) { // Beta-cutoff przeciwnik i tak nie pozwoli nam na ten ruch
            return beta;
        }

        if (score > alpha) {//nowy lepszy ruch
            alpha = score;
        }
    }

    if (!any_legal_move) { //oznacza ze dostal mata
        if (is_in_check_friendly(pos)) return -30000 + (root_depth - depth);
        return 0;
    }

    return alpha;
}


//=========================do sort moves================================================================
int Search::score_move(Position& pos, Move move) {

    if (move.flags & 4) {
        int friendly = pos.piece_on_square(move.from);
        int enemy = pos.piece_on_square(move.to);

        return 10000 + (pieceValues[enemy] * 10) - pieceValues[friendly];
    }

    // Jeœli ruch koñczy siê na polu z wysokim bonusem PST, daj mu punkty
    int piece = pos.piece_on_square(move.from);
    return pst[0][piece][move.to];
}

