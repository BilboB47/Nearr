
#include "search.hpp"

bool Search::is_repetition(Position& pos) {
    for (int i = 0; i < history_size - 1; i++) {
        if (history_stack[i] == pos.zobristKey) {
            return true;
        }
    }
    return false;
}




Move Search::get_best_move(Position& pos, int max_depth) {
    Move best_overall_move;

    for (int current_depth = 1; current_depth <= max_depth; current_depth++) {

        Move best_move_this_iter;
        int best_score = -33000;
        int alpha = -33000;
        int beta = 33000;

        Move moves[256];
        int count = generateMoves(pos, moves);

        int scores[256];
        for (int i = 0; i < count; i++) scores[i] = score_move(pos, moves[i]);

        bool any_legal_move = false;

        for (int i = 0; i < count; i++) {

            UndoInfo info = pos.make_move(moves[i]);
            if (is_in_check_enemy(pos)) {
                pos.unmake_move(moves[i], info);
                continue;
            }
            any_legal_move = true;

            int score = -alpha_beta(pos, current_depth - 1, -beta, -alpha, current_depth);
            pos.unmake_move(moves[i], info);

            if (score > best_score) {
                best_score = score;
                best_move_this_iter = moves[i];
            }
            if (score > alpha) alpha = score;
        }

        if (!any_legal_move)return Move();

        best_overall_move = best_move_this_iter;

        if (best_score > 20000) break;
    }

    return best_overall_move;
}

int Search::alpha_beta(Position& pos, int depth, int alpha, int beta, int root_depth) {
    nodes++;

    if (depth < root_depth && is_repetition(pos)) return 0;
    if (depth <= 0) return evaluate(pos);

    Move moves[256];
    int count = generateMoves(pos, moves);
    int scores[256];

    for (int i = 0; i < count; i++) {
        scores[i] = score_move(pos, moves[i]);
    }

    bool any_legal_move = false;

    for (int i = 0; i < count; i++) {

        for (int j = i + 1; j < count; j++) {
            if (scores[j] > scores[i]) {
                std::swap(scores[i], scores[j]);
                std::swap(moves[i], moves[j]);
            }
        }

        UndoInfo info = pos.make_move(moves[i]);
        if (is_in_check_enemy(pos)) {
            pos.unmake_move(moves[i], info);
            continue;
        }
        any_legal_move = true;

        int score = -alpha_beta(pos, depth - 1, -beta, -alpha, root_depth);

        pos.unmake_move(moves[i], info);

        if (score >= beta) {
            return beta;
        }

        if (score > alpha) {
            alpha = score;
        }
    }

    if (!any_legal_move) {
        if (is_in_check_friendly(pos)) return -30000 + (root_depth - depth);
        return 0;
    }

    return alpha;
}


int Search::score_move(Position& pos, Move move) {
    // 1. Priorytet dla bicia (MVV-LVA)
    if (move.flags & 4) {
        int attacker = pos.piece_on_square(move.from);
        int victim = pos.piece_on_square(move.to);

        // Formu³a: 10 * wartoœæ ofiary - wartoœæ atakuj¹cego
        // Dziêki temu PION bije HETMANA ma najwy¿szy priorytet
        return 10000 + (pieceValues[victim] * 10) - pieceValues[attacker];
    }

    // 2. Bonusy z Twoich tabel PST
    // Jeœli ruch koñczy siê na polu z wysokim bonusem PST, daj mu punkty
    int piece = pos.piece_on_square(move.from);
    return pst[0][piece][move.to];
}

