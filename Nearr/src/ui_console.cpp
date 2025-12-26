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


Color choose_color()
{
    string choice;
    while (true) {
        std::cout << endl <<"Wybierz kolor (w - biale, b - czarne): ";
        std::cin >> choice;

        if (choice == "w" || choice == "W") {
            return WHITE;  
        }
        if (choice == "b" || choice == "B") {
            return BLACK; 
        }

        std::cout << "Niepoprawny wybor. Sprobuj ponownie." << std::endl;
    }
    return WHITE;
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

void printVictory() {
    cout << " __      __                            .__                  \n";
    cout << "/  \\    /  \\___.__. ________________  |  |   ____   ______  \n";
    cout << "\\   \\/\\/   <   |  |/ ___\\_  __ \\__  \\ |  | _/ __ \\ /  ___/  \n";
    cout << " \\        / \\___  / /_/  >  | \\// __ \\|  |_\\  ___/ \\___ \\   \n";
    cout << "  \\__/\\  /  / ____\\___  /|__|  (____  /____/\\___  >____  >  \n";
    cout << "       \\/   \\/   /_____/            \\/          \\/     \\/   \n";
}
void printDefeat() {
    cout << "__________                                          .__                  \n";
    cout << "\\______   \\_______________ ____   ________________  |  |   ____   ______  \n";
    cout << " |     ___/\\_  __ \\___   // __ \\ / ___\\_  __ \\__  \\ |  | _/ __ \\ /  ___/  \n";
    cout << " |    |     |  | \\//    /\\  ___// /_/  >  | \\// __ \\|  |_\\  ___/ \\___ \\   \n";
    cout << " |____|     |__|  /_____ \\\\___  >___  /|__|  (____  /____/\\___  >____  >  \n";
    cout << "                        \\/    \\/_____/            \\/          \\/     \\/   \n";
}
void printDraw() {
    cout << "__________                 .__          \n";
    cout << "\\______   \\ ____   _____ |__| ______  \n";
    cout << " |       _// __ \\ /     \\|  |/  ___/  \n";
    cout << " |    |   \\  ___/|  Y Y  \\  |\\___ \\   \n";
    cout << " |____|_  /\\___  >__|_|  /__/____  >  \n";
    cout << "        \\/     \\/      \\/        \\/   \n";
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


    Search engine;
    const int depth = 7;


    Color UserColor = choose_color();
    Color CurrColor;

    while (1){

        CurrColor = (pos.isWhiteMove) ? WHITE : BLACK;

        cout << endl << "=================================================================" << endl;
        cout << "{NR RUCHU: " << pos.moveNumber << "} {KOLOR: ";
        cout << (CurrColor == WHITE ? "BIALE " : "CZARNE ");
        cout << (UserColor == CurrColor ? "(gracz)" : "(komputer)") << "}";
        cout << endl <<"=================================================================" << endl;

        //----------------GRACZ------------------------------------------------------------
        if (UserColor == CurrColor) { 
            //pseudolegalne
            Move moves[256];
            int size_moves = generateMoves(pos, moves);
            //legalne (filtr)
            vector<Move> legal_moves;
            for (int i = 0; i < size_moves; i++) {
            UndoInfo info = pos.make_move(moves[i]);

            if (!is_in_check_enemy(pos)) {
                legal_moves.push_back(moves[i]);
            }

            pos.unmake_move(moves[i], info);
        }

            if (legal_moves.size()==0)break;//brak ruchu
            
            //wybór ruchu
            cout << endl;
            Move UserMove = choose_move(legal_moves);
            pos.make_move(UserMove);
        }
        //----------------SILNIK------------------------------------------------------------
        else { 
            std::cout << std::endl << "Silnik mysli..." << std::endl << std::endl;
            Move bestMove = engine.get_best_move(pos, depth);

            if (bestMove.from == A1 && bestMove.to == A1)break;//brak ruchu
            
            cout << "Ruch silnika: " << moveToString(bestMove) << "                      Nodes: " <<  engine.nodes <<endl;
            pos.make_move(bestMove);

        }

        pos.print_board();
    }


    //---------stan końcowy gry--------------------------------------------------------------------------
    if (!is_in_check_friendly(pos) && !is_in_check_enemy(pos))printDraw();
    if (UserColor == CurrColor){
        printDefeat();
    }
    else {
        printVictory();
    }

}


