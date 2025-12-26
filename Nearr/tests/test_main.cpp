//bool compare_position(const Position& p1, const Position& p2) {
//    for (int i = 0; i < 14; i++) {
//        if (p1.bitBoard[i] != p2.bitBoard[i]) return false;
//    }
//
//    if (p1.isWhiteMove != p2.isWhiteMove) return false;
//    if (p1.castlingRights != p2.castlingRights) return false;
//    if (p1.enPassantSquare != p2.enPassantSquare) return false;
//    if (p1.halfmoveClock != p2.halfmoveClock) return false;
//    if (p1.moveNumber != p2.moveNumber) return false;
//    if (p1.zobristKey != p2.zobristKey) return false;
//
//    return true;
//}
//void test_unmake() {
//    Position pos;
//    pos.set_position_FEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
//
//    pos.print_board();
//    cout << endl << pos.zobristKey << endl;
//
//    Move moves[256];
//    int count = generateMoves(pos, moves);
//    UndoInfo info = pos.make_move(moves[1]);
//
//    pos.print_board();
//    cout << endl << pos.zobristKey << endl;
//
//    pos.unmake_move(moves[1], info);
//    pos.print_board();
//    cout << endl << pos.zobristKey << endl;
//
//    Position posC;
//    posC.set_position_FEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
//
//    cout << compare_position(pos, posC);
//}
//bool test_compree(const Position& pos, const Position& copy2) {
//    bool isCorrect = true;
//
//    if (!compare_position(pos, copy2)) {
//        isCorrect = false;
//
//        // Sprawdzanie Bitboardów
//        for (int b = 0; b < 14; b++) {
//            if (pos.bitBoard[b] != copy2.bitBoard[b]) {
//                std::cout << " -> Bitboard [" << b << "] siê ró¿ni!" << std::endl;
//                // Wypisywanie hex z zerami u³atwia lokalizacjê bitu (np. 0x40 to 0000000000000040)
//                std::cout << "    By³o: 0x" << std::hex << std::setw(16) << std::setfill('0') << copy2.bitBoard[b] << std::endl;
//                std::cout << "    Jest: 0x" << std::hex << std::setw(16) << std::setfill('0') << pos.bitBoard[b] << std::dec << std::endl;
//            }
//        }
//
//        // Sprawdzanie En Passant
//        if (pos.enPassantSquare != copy2.enPassantSquare) {
//            std::cout << " -> EnPassant siê ró¿ni!" << std::endl;
//            std::cout << "    By³o: " << (int)copy2.enPassantSquare << " | Jest: " << (int)pos.enPassantSquare << std::endl;
//        }
//
//        // Sprawdzanie Zobrist
//        if (pos.zobristKey != copy2.zobristKey) {
//            std::cout << " -> ZobristKey siê ró¿ni!" << std::endl;
//            std::cout << "    By³o: 0x" << std::hex << copy2.zobristKey << " | Jest: 0x" << pos.zobristKey << std::dec << std::endl;
//        }
//
//        // Sprawdzanie Roszad
//        if (pos.castlingRights != copy2.castlingRights) {
//            std::cout << " -> CastlingRights siê ró¿ni!" << std::endl;
//            std::cout << "    By³o: " << (int)copy2.castlingRights << " | Jest: " << (int)pos.castlingRights << std::endl;
//        }
//
//        // Sprawdzanie Metadanych
//        if (pos.isWhiteMove != copy2.isWhiteMove) {
//            std::cout << " -> isWhiteMove siê ró¿ni! (KRYTYCZNE)" << std::endl;
//        }
//        if (pos.moveNumber != copy2.moveNumber) {
//            std::cout << " -> moveNumber siê ró¿ni!" << std::endl;
//        }
//
//        std::cout << "--------------------------------------\n" << std::endl;
//    }
//
//    return isCorrect;
//}
//
//uint64_t perft_fast(Position& pos, int depth) {
//    if (depth == 0) {
//        return 1;
//    }
//
//
//    uint64_t nodes = 0;
//    Move moves[256];
//    int count = generateMoves(pos, moves);
//
//    for (int i = 0; i < count; i++) {
//        UndoInfo info = pos.make_move(moves[i]);
//
//        // Sprawdzamy legalnoœæ (król nie mo¿e byæ bity)
//        if (!is_in_check_enemy(pos)) {
//            nodes += perft_fast(pos, depth - 1);
//        }
//
//        pos.unmake_move(moves[i], info);
//    }
//    return nodes;
//}
//void perft_divider(Position& pos, int depth) {
//    uint64_t total_nodes = 0;
//    Move moves[256];
//    int count = generateMoves(pos, moves);
//
//    std::cout << "--- Perft Divider Depth " << depth << " ---" << std::endl;
//
//    for (int i = 0; i < count; i++) {
//        UndoInfo info = pos.make_move(moves[i]);
//
//        if (is_in_check_enemy(pos)) {
//            pos.unmake_move(moves[i], info);
//            continue;
//        }
//
//        // Tutaj obliczamy pod-wêz³y dla tego konkretnego ruchu
//        uint64_t nodes = perft_fast(pos, depth - 1);
//        pos.unmake_move(moves[i], info);
//
//        total_nodes += nodes;
//    }
//
//    std::cout << "=================================" << std::endl;
//    std::cout << "Suma wszystkich wezlow: " << total_nodes << std::endl;
//}
//
//void run_benchmark(Position& pos, int depth) {
//    std::cout << "Rozpoczynam test wydajnosci (Perft depth " << depth << ")..." << std::endl;
//
//    auto start = std::chrono::high_resolution_clock::now();
//
//    uint64_t nodes = perft_fast(pos, depth);
//
//    auto end = std::chrono::high_resolution_clock::now();
//    std::chrono::duration<double> elapsed = end - start;
//
//    double nps = nodes / elapsed.count();
//
//    std::cout << "---------------------------------" << std::endl;
//    std::cout << "Wynik: " << nodes << " wezlow" << std::endl;
//    std::cout << "Czas:  " << elapsed.count() << " s" << std::endl;
//    std::cout << "NPS:   " << nps / 1000000.0 << " MNPS (milionow wezlow/s)" << std::endl;
//    std::cout << "---------------------------------" << std::endl;
//}
