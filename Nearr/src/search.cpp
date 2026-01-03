
#include "search.hpp"
#include "bitboard_utils.hpp"

void Search::clear_tt()
{
    memset(TT_table, 0, sizeof(TTEntry) * TT_SIZE);

    history_size = 0;
    memset(killerMoves, 0, sizeof(killerMoves));
    memset(history_stack, 0, sizeof(history_stack));
}

//=========================SEARCH================================================================
Move Search::get_best_move(Position& pos, int max_depth) {
    this->nodes = 0;
    Move global_best_move;

    // ---reset killer---
    memset(killerMoves, 0, sizeof(killerMoves));

    //-----iterative deeping-------------------------------
    for (int current_depth = 1; current_depth <= max_depth; current_depth++) {

        Move best_move_this_iteration;

        int best_score = -33000;
        int alpha = -33000;
        int beta = 33000;

        Move moves[256];
        int count = generateMoves(pos, moves);
        int scores[256];

        uint64_t index = pos.zobristKey & (TT_SIZE - 1);//index pozycji

        Move tt_move;
        if (TT_table[index].key == pos.zobristKey) {//przypisz wczesniej policzony ruch jesli taki jest
            tt_move = TT_table[index].bestMove;
        }

        for (int i = 0; i < count; i++) {
            if ((tt_move.from != 0 && tt_move.to != 0) && moves[i] == tt_move) { //jest to ten sam ruch co z tt
                scores[i] = 2000000;
            }
            else {
                scores[i] = score_move(pos, moves[i], 0); //0 bo jest to najp�ytszy killer
            }
        }

        bool any_legal_move = false;

        for (int i = 0; i < count; i++) { //kazdego ruchu

            for (int j = i + 1; j < count; j++) {//move orderning
                if (scores[j] > scores[i]) {
                    std::swap(scores[i], scores[j]);
                    std::swap(moves[i], moves[j]);
                }
            }

            //----filtr legalno�ci--------------------
            UndoInfo info = pos.make_move(moves[i]);

            Color attacking_color = (pos.isWhiteMove) ? WHITE : BLACK;
            uint64_t king_bb = (pos.isWhiteMove) ? pos.bitBoard[BLACK_KING] : pos.bitBoard[WHITE_KING];
            get_lsb(&king_bb);

            if (is_square_attacked(pos, get_lsb(&king_bb), attacking_color)) {//is_in_check_enemy(pos)
                pos.unmake_move(moves[i], info);
                continue;
            }
            any_legal_move = true;
            //---------------------------------------

            //alpha beta pruning glebiej
            int score = -alpha_beta(pos, current_depth - 1, -beta, -alpha, current_depth, 1);
            pos.unmake_move(moves[i], info);

            //new the best move na tej glebokosci
            if (score > best_score) {
                best_score = score;
                best_move_this_iteration = moves[i];
            }
            if (score > alpha) alpha = score;
        }

        if (!any_legal_move) break;
        global_best_move = best_move_this_iteration; //zawsze zmienia bo g��biej=lepiej
    }

    return global_best_move;
}

int Search::alpha_beta(Position& pos, int depth, int alpha, int beta, int root_depth, int ply) {
    nodes++;

    if (is_repetition(pos.zobristKey)) { //czy jest to powielenie pozycji daje 0 czyli remis (zasada powt�zenia pozycji)
        return 0;
    }

    //--------TT---------------------------
    uint64_t index = pos.zobristKey & (TT_SIZE - 1);//index w tablicy TT

    TTEntry& tt_value = TT_table[index]; //zawrto�� TT takiej pozycji
    Move tt_move = Move();

    if (tt_value.key == pos.zobristKey) { //je�li to jest faktycznie taka pozycja (index mo�e zawiera� kolizje)
        tt_move = tt_value.bestMove;

        if (tt_value.depth >= depth) {
            int score = tt_value.score;
            //---maty---
            if (score > 29000) score -= ply;
            if (score < -29000) score += ply;

            //---flag na podstawei score---
            if (tt_value.flag == TTFlag::Exact) return score; //dokladnie taki sam
            if (tt_value.flag == TTFlag::Alpha && score <= alpha) return alpha; // G�rna granica (fail-low): wynik nie poprawi alpha
            if (tt_value.flag == TTFlag::Beta && score >= beta) return beta;    // Dolna granica (fail-high): przeciwnik ma lepsz� opcj�
        }
    }

    if (depth <= 0) return quiescence(pos, alpha, beta); //licz dalej tylko glo�ne ruchy

    //-------liczenie wszytkich ruch�w--------------------------------------------------
    Move moves[256];
    int count = generateMoves(pos, moves);
    int scores[256];

    //---danie score by robic move ordering---
    for (int i = 0; i < count; i++) {
        if (moves[i] == tt_move) {
            scores[i] = 2000000;
        }
        else {
            scores[i] = score_move(pos, moves[i], ply);
        }
    }

    bool any_legal_move = false;

    int original_alpha = alpha; //decyduje o fladze TT
    Move best_move_in_node = Move();

    //----move orderming--------------
    for (int i = 0; i < count; i++) {
        for (int j = i + 1; j < count; j++) {
            if (scores[j] > scores[i]) {
                std::swap(scores[i], scores[j]);
                std::swap(moves[i], moves[j]);
            }
        }

        UndoInfo info = pos.make_move(moves[i]);
        Color attacking_color = (pos.isWhiteMove) ? WHITE : BLACK;
        uint64_t king_bb = (pos.isWhiteMove) ? pos.bitBoard[BLACK_KING] : pos.bitBoard[WHITE_KING];
        get_lsb(&king_bb);

        if (is_square_attacked(pos, get_lsb(&king_bb), attacking_color)) {//is_in_check_enemy(pos)
            //if (is_in_check_enemy(pos)) {
            pos.unmake_move(moves[i], info);
            continue;
        }
        any_legal_move = true;

        int score = -alpha_beta(pos, depth - 1, -beta, -alpha, root_depth, ply + 1);
        pos.unmake_move(moves[i], info);

        if (score >= beta) {
            //-----zapis killera-------------------------------------
            if (!(moves[i].flags & 4) && ply < 64) {//ply < 64 index (chociaz i tak nie odpali sie to na depth 64)
                killerMoves[ply][1] = killerMoves[ply][0];
                killerMoves[ply][0] = moves[i];
            }

            int store_score = beta;
            //---order mata---
            if (store_score > 29000) store_score += ply;
            if (store_score < -29000) store_score -= ply;

            tt_value.key = pos.zobristKey;
            tt_value.score = store_score;
            tt_value.flag = TTFlag::Beta;
            tt_value.depth = depth;
            tt_value.bestMove = moves[i];

            return beta;
        }

        if (score > alpha) {
            alpha = score;
            best_move_in_node = moves[i];
        }
    }

    if (!any_legal_move) {
        if (is_in_check_friendly(pos)) return -30000 + ply;
        return 0;
    }

    int store_score = alpha;
    //---order mata---
    if (store_score > 29000) store_score += ply;
    if (store_score < -29000) store_score -= ply;

    tt_value.key = pos.zobristKey;
    tt_value.score = store_score;
    tt_value.flag = (alpha <= original_alpha) ? TTFlag::Alpha : TTFlag::Exact;
    tt_value.depth = depth;
    tt_value.bestMove = best_move_in_node;

    return alpha;
}

int Search::quiescence(Position& pos, int alpha, int beta) {
    int standby_score = evaluate(pos);

    if (standby_score >= beta) return beta;
    if (standby_score > alpha) alpha = standby_score;

    Move moves[128];
    int count = generateCaptures(pos, moves);

    for (int i = 0; i < count; i++) {
        UndoInfo info = pos.make_move(moves[i]);

        Color attacking_color = (pos.isWhiteMove) ? WHITE : BLACK;
        uint64_t king_bb = (pos.isWhiteMove) ? pos.bitBoard[BLACK_KING] : pos.bitBoard[WHITE_KING];
        get_lsb(&king_bb);

        if (is_square_attacked(pos, get_lsb(&king_bb), attacking_color)) {//is_in_check_enemy(pos)
            //if (is_in_check_enemy(pos)) {
            pos.unmake_move(moves[i], info);
            continue;
        }

        int score = -quiescence(pos, -beta, -alpha);

        pos.unmake_move(moves[i], info);

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }

    return alpha;
}

bool Search::is_repetition(uint64_t current_key) {
    for (int i = 0; i < history_size - 1; i++) {
        if (history_stack[i] == current_key) {
            return true;
        }
    }
    return false;
}


//=========================do sort moves================================================================
int Search::score_move(Position& pos, Move move, int ply) {

    if (move.flags & 4) {//jest to bicie 
        int friendly = pos.piece_on_square(move.from);
        int enemy = pos.piece_on_square(move.to);
        //Bicia maj� du�e value + w zale�no�ci co bije co
        return 1000000 + (pieceValues[enemy] * 10) - pieceValues[friendly];
    }

    //-----killer---------------------------
    if (ply < 64) {
        if (move == killerMoves[ply][0]) return 900000;
        if (move == killerMoves[ply][1]) return 800000;
    }

    //jak nie jest to bicie to daje zwykle pst (by po czym� sortowalo)
    int piece = pos.piece_on_square(move.from);
    return pst[0][piece][move.to];
}