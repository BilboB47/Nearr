#include "ui_console.hpp"


char get_piece_from_number(uint8_t n) {
    char piece = ' ';
    switch (n) {
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
    default: piece = ' '; break;
    }
    return piece;
}

std::string squareToString(int index) {
    int file = index % 8;  // kolumna (A–H)
    int rank = index / 8;  // rząd (1–8)
    char fileChar = 'A' + file;
    char rankChar = '1' + rank;
    return std::string() + fileChar + rankChar;
}

std::string moveToString(const Move m) {
    std::string fromSquare = squareToString(m.from);
    std::string toSquare = squareToString(m.to);

    return fromSquare+toSquare;
}

bool is_choose_white() {
    char choice;
    while (true) {
        std::cout << "Wybierz kolor (w - biale, b - czarne): ";
        std::cin >> choice;

        if (choice == 'w' || choice == 'W') {
            return true;  
        }
        if (choice == 'b' || choice == 'B') {
            return false; 
        }

        std::cout << "Niepoprawny wybor. Sprobuj ponownie." << std::endl;
    }
}

Move choose_move(const std::vector<Move>& legal_moves) {
    std::string userInput;

    while (true) {
        std::cout << "Podaj ruch (np. E2E4): ";
        std::cin >> userInput;

        for (char& c : userInput) {
            c = std::toupper((unsigned char)c);
        }

        std::vector<Move> matching_moves;
        for (const auto& m : legal_moves) {
            if (moveToString(m) == userInput) {
                matching_moves.push_back(m);
            }
        }

        if (matching_moves.size() == 1) {
            return matching_moves[0]; // Tylko jeden pasujący ruch (zwykły ruch)
        }
        else if (matching_moves.size() > 1) {
            // Mamy promocję! Szukamy ruchu z flagą Hetmana (Q)
            for (const auto& m : matching_moves) {
                int f = m.flags; // zakładam że obiekt Move ma pole flags
                if (f == FLAG_PROMOTION_Q || f == FLAG_PROMOTION_CAPTURE_Q) {
                    return m;
                }
            }
            // Jeśli z jakiegoś powodu nie znaleźliśmy Q, zwróć pierwszy lepszy
            return matching_moves[0];
        }

        // 3. Jeśli matching_moves jest puste - błąd
        std::cout << "Niepoprawny ruch. Dostepne ruchy: " << std::endl;
        for (const auto& m : legal_moves) {
            std::cout << moveToString(m) << " ";
        }
        std::cout << std::endl;
    }
}



void game() {

    initAttackTables();
    init_zobrist_keys();
    init_pst();

    Position pos;
    pos.set_position_FEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    pos.set_eval_state();
    pos.set_phase_state();

    pos.print_board();
    std::cout << "=====================================================================" << std::endl;


    Search engine;

    int depth = 7;

    Move UserMove;

    if (is_choose_white()){
        Move moves[256];
        int size_moves = generateMoves(pos, moves);

        vector<Move> legal_moves;

        for (int i = 0; i < size_moves; i++) {
            UndoInfo info = pos.make_move(moves[i]);

            if (!is_in_check_enemy(pos)) {
                legal_moves.push_back(moves[i]);
            }

            pos.unmake_move(moves[i], info);
        }


        UserMove = choose_move(legal_moves);
        pos.make_move(UserMove);
        pos.print_board();
    }

    while (1){
        std::cout << std::endl << "Silnik mysli..." << std::endl << std::endl;

        Move bestMove = engine.get_best_move(pos, depth);
        std::cout << "=====================================================================" << std::endl;
        cout << "ruch silnika: " << moveToString(bestMove) << "        nodes: " <<  engine.nodes <<endl;
        pos.make_move(bestMove);
        
        pos.print_board();
        std::cout << "=====================================================================" << std::endl;


        Move moves[256];
        int size_moves = generateMoves(pos, moves);


        vector<Move> legal_moves;

        for (int i = 0; i < size_moves; i++) {
            UndoInfo info = pos.make_move(moves[i]);

            if (!is_in_check_enemy(pos)) {
                legal_moves.push_back(moves[i]);
            }

            pos.unmake_move(moves[i], info);
        }


        if (bestMove.from == A1 && bestMove.to == A1 || legal_moves.size() == 0)break;
        
        UserMove = choose_move(legal_moves);
        
        pos.make_move(UserMove);

        pos.print_board();

    }

    if (is_in_check_friendly(pos))cout << "Przegrales" << std::endl << std::endl;
    if (is_in_check_enemy(pos))cout << "Wygrales" << std::endl << std::endl;
    if (!is_in_check_friendly(pos) && !is_in_check_enemy(pos))cout << "Remis" << std::endl << std::endl;


}
