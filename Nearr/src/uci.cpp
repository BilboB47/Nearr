#include "uci.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>

using namespace std;

inline std::string SToString(int index) {
    int file = index % 8;  // kolumna (a–h)
    int rank = index / 8;  // rz¹d (1–8)
    char fileChar = 'a' + file;
    char rankChar = '1' + rank;
    return std::string({ fileChar, rankChar });
}

inline std::string MToString(const Move m) {

    std::string fromSquare = SToString(m.from);
    std::string toSquare = SToString(m.to);
    std::string res = fromSquare + toSquare;

    // Obs³uga Twoich flag promocji (8-11 czysta, 12-15 z biciem)
    if (m.flags == 8 || m.flags == 12) res += 'n';
    else if (m.flags == 9 || m.flags == 13) res += 'b';
    else if (m.flags == 10 || m.flags == 14) res += 'r';
    else if (m.flags == 11 || m.flags == 15) res += 'q';

    return res;
}



Move parse_move_string(Position& pos, string move_str) {
    Move moves[256];
    // Generator tworzy pe³ne obiekty Move ze wszystkimi flagami
    int size = generateMoves(pos, moves);

    for (int i = 0; i < size; i++) {
        // moveToString zamienia Twój obiekt Move na tekst "e2e4"
        // Jeœli tekst siê zgadza, zwracamy PE£NY obiekt z flagami
        
        if (MToString(moves[i]) == move_str) {
            return moves[i];
        }
    }

    // Jeœli nie znaleziono ruchu (np. nielegalny ruch z GUI)
    return Move{ A1, A1, 0 };
}

void uci_loop() {
    // Inicjalizacja silnika
    initAttackTables();
    init_zobrist_keys();
    init_pst();

    Position pos;
    Search engine;
    string line, command;

    // --- £ATWE USTAWIANIE G£ÊBOKOŒCI ---
    int target_depth = 8; // Zmieñ tutaj, aby domyœlnie ustawiæ inn¹ g³êbokoœæ
    // -----------------------------------

    while (getline(cin, line)) {
        stringstream ss(line);
        ss >> command;

        if (command == "uci") {
            cout << "id name Nearr Chess Engine" << endl;
            cout << "id author BilboB47" << endl;

            // --- TA LINIA POZWOLI PYTONOWI ZOBACZYÆ OPCJÊ DEPTH ---
            cout << "option name depth type spin default 7 min 1 max 20" << endl;

            cout << "uciok" << endl;
        }
        else if (command == "isready") {
            cout << "readyok" << endl;
        }
        else if (command == "ucinewgame") {
            engine.clear_tt();
            pos.set_position_FEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        }
        else if (command == "setoption") {
            string name_token, option_name, value_token;
            ss >> name_token >> option_name;
            if (option_name == "depth") {
                ss >> value_token >> target_depth;
            }
        }
        else if (command == "position") {
            string sub;
            ss >> sub;
            engine.history_size = 0;

            if (sub == "startpos") {
                pos.set_position_FEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
                ss >> sub;
            }
            else if (sub == "fen") {
                string fen = "";
                while (ss >> sub && sub != "moves") {
                    fen += sub + " ";
                }
                pos.set_position_FEN(fen);
            }

            engine.history_stack[engine.history_size++] = pos.zobristKey;

            if (sub == "moves") {
                string m_str;
                while (ss >> m_str) {
                    Move m = parse_move_string(pos, m_str);
                    if (m.from != A1 || m.to != A1) {
                        pos.make_move(m);
                        engine.history_stack[engine.history_size++] = pos.zobristKey;
                    }
                }
            }
            pos.set_eval_state();
            pos.set_phase_state();
        }
        else if (command == "go") {
            string sub;
            int depth_to_use = target_depth; // wartoœæ domyœlna (np. 8)

            // Ta pêtla przeszukuje liniê w poszukiwaniu s³owa "depth"
            while (ss >> sub) {
                if (sub == "depth") {
                    ss >> depth_to_use; // przypisuje liczbê po s³owie depth
                }
            }

            auto start = std::chrono::high_resolution_clock::now();

            // KLUCZOWA ZMIANA: Przekazujemy depth_to_use, a nie target_depth
            Move bestMove = engine.get_best_move(pos, depth_to_use);

            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;
            double seconds = elapsed.count();
            long long nps = (seconds > 0.001) ? (long long)(engine.nodes / seconds) : 0;

            // Wypisujemy info z faktyczn¹ g³êbokoœci¹ dla Lichess
            cout << "info depth " << depth_to_use
                << " nodes " << engine.nodes
                << " nps " << nps
                << " time " << (int)(seconds * 1000) << endl;

            cout << "bestmove " << MToString(bestMove) << endl;
        }
        else if (command == "quit") {
            break;
        }
    }
}
