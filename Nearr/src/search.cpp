
#include "search.hpp"

Move Search::get_best_move(Position& pos, int max_depth) {
    Move best_overall_move;

    // Iterative Deepening to klucz! 
    // Najpierw szukamy mata w 1, potem w 2, potem w 3...
    for (int current_depth = 1; current_depth <= max_depth; current_depth++) {

        Move best_move_this_iter;
        int best_score = -33000;
        int alpha = -33000;
        int beta = 33000;

        Move moves[256];
        int count = generateMoves(pos, moves);

        // DODAJ SORTOWANIE TUTAJ TAK SAMO JAK W ALPHA-BETA!
        int scores[256];
        for (int i = 0; i < count; i++) scores[i] = score_move(pos, moves[i]);

        for (int i = 0; i < count; i++) {
            // Selection sort... (kod jak w Twoim alpha_beta)

            UndoInfo info = pos.make_move(moves[i]);
            if (is_in_check_enemy(pos)) {
                pos.unmake_move(moves[i], info);
                continue;
            }

            // Wa¿ne: przekazujemy current_depth
            int score = -alpha_beta(pos, current_depth - 1, -beta, -alpha, current_depth);
            pos.unmake_move(moves[i], info);

            if (score > best_score) {
                best_score = score;
                best_move_this_iter = moves[i];
            }
            if (score > alpha) alpha = score;
        }

        best_overall_move = best_move_this_iter;

        // Jeœli znaleŸliœmy mata, mo¿emy przestaæ szukaæ g³êbiej
        if (best_score > 20000) break;
    }

    return best_overall_move;
}

int Search::alpha_beta(Position& pos, int depth, int alpha, int beta, int root_depth) {
    if (depth == 0) return evaluate(pos);

    Move moves[256];
    int count = generateMoves(pos, moves);

    // 1. Obliczamy punkty dla wszystkich wygenerowanych ruchów
    int scores[256];
    for (int i = 0; i < count; i++) {
        scores[i] = score_move(pos, moves[i]);
    }

    int legal_moves_count = 0;

    for (int i = 0; i < count; i++) {

        // 2. SELECTION SORT (Wybieramy najlepszy ruch spoœród pozosta³ych)
        for (int j = i + 1; j < count; j++) {
            if (scores[j] > scores[i]) {
                // Zamieniamy miejscami punkty i odpowiadaj¹ce im ruchy
                std::swap(scores[i], scores[j]);
                std::swap(moves[i], moves[j]);
            }
        }

        UndoInfo info = pos.make_move(moves[i]);
        if (is_in_check_enemy(pos)) {
            pos.unmake_move(moves[i], info);
            continue;
        }

        legal_moves_count++;
        int score = -alpha_beta(pos, depth - 1, -beta, -alpha, root_depth);
        pos.unmake_move(moves[i], info);

        if (score >= beta) {
            return beta; // Cut-off (dziêki sortowaniu wyst¹pi znacznie szybciej!)
        }
        if (score > alpha) {
            alpha = score;
        }
    }

    if (legal_moves_count == 0) {
        if (is_in_check_friendly(pos)) return -MATE_VALUE + (root_depth - depth);
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


