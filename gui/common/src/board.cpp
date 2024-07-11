#include "common/board.hpp"

#include <cmath>
#include <utility>
#include <cassert>

namespace board {
    static const int NODE_POSITIONS[24][2] {
        { 2, 2 },
        { 5, 2 },
        { 8, 2 },
        { 3, 3 },
        { 5, 3 },
        { 7, 3 },
        { 4, 4 },
        { 5, 4 },
        { 6, 4 },
        { 2, 5 },
        { 3, 5 },
        { 4, 5 },
        { 6, 5 },
        { 7, 5 },
        { 8, 5 },
        { 4, 6 },
        { 5, 6 },
        { 6, 6 },
        { 3, 7 },
        { 5, 7 },
        { 7, 7 },
        { 2, 8 },
        { 5, 8 },
        { 8, 8 }
    };

    static Idx index_from_string(std::string_view string) {
        if (string == "a7") return 0;
        else if (string == "d7") return 1;
        else if (string == "g7") return 2;
        else if (string == "b6") return 3;
        else if (string == "d6") return 4;
        else if (string == "f6") return 5;
        else if (string == "c5") return 6;
        else if (string == "d5") return 7;
        else if (string == "e5") return 8;
        else if (string == "a4") return 9;
        else if (string == "b4") return 10;
        else if (string == "c4") return 11;
        else if (string == "e4") return 12;
        else if (string == "f4") return 13;
        else if (string == "g4") return 14;
        else if (string == "c3") return 15;
        else if (string == "d3") return 16;
        else if (string == "e3") return 17;
        else if (string == "b2") return 18;
        else if (string == "d2") return 19;
        else if (string == "f2") return 20;
        else if (string == "a1") return 21;
        else if (string == "d1") return 22;
        else if (string == "g1") return 23;
        else return 0;
    }

    static std::string_view string_from_index(Idx index) {
        switch (index) {
            case 0: return "a7";
            case 1: return "d7";
            case 2: return "g7";
            case 3: return "b6";
            case 4: return "d6";
            case 5: return "f6";
            case 6: return "c5";
            case 7: return "d5";
            case 8: return "e5";
            case 9: return "a4";
            case 10: return "b4";
            case 11: return "c4";
            case 12: return "e4";
            case 13: return "f4";
            case 14: return "g4";
            case 15: return "c3";
            case 16: return "d3";
            case 17: return "e3";
            case 18: return "b2";
            case 19: return "d2";
            case 20: return "f2";
            case 21: return "a1";
            case 22: return "d1";
            case 23: return "g1";
            default: return "";
        }
    }

    MuhleBoard::MuhleBoard(const MoveCallback& move_callback)
        : move_callback(move_callback) {
        legal_moves = generate_moves();
    }

    void MuhleBoard::update(bool user_input) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(350.0f, 350.0f));

        if (ImGui::Begin("Board")) {
            const ImVec2 canvas_p0 {ImGui::GetCursorScreenPos()};
            ImVec2 canvas_size {ImGui::GetContentRegionAvail()};

            canvas_size.x = std::max(canvas_size.x, 350.0f);
            canvas_size.y = std::max(canvas_size.y, 350.0f);

            const ImVec2 canvas_p1 {ImVec2(canvas_p0.x + canvas_size.x, canvas_p0.y + canvas_size.y)};

            ImDrawList* draw_list {ImGui::GetWindowDrawList()};

            const float UNIT {canvas_size.x < canvas_size.y ? (canvas_p1.x - canvas_p0.x) / 10.0f : (canvas_p1.y - canvas_p0.y) / 10.0f};
            const ImVec2 OFFSET {ImVec2(canvas_p0.x, canvas_p0.y)};

            const ImColor COLOR {ImColor(200, 200, 200)};
            const float THICKNESS {2.0f};

            board_unit = UNIT;
            board_offset = OFFSET;

            draw_list->AddRectFilled(canvas_p0, canvas_p1, ImColor(45, 45, 45));

            draw_list->AddRect(ImVec2(2.0f * UNIT + OFFSET.x, 8.0f * UNIT + OFFSET.y), ImVec2(8.0f * UNIT + OFFSET.x, 2.0f * UNIT + OFFSET.y), COLOR, 0.0f, 0, THICKNESS);
            draw_list->AddRect(ImVec2(3.0f * UNIT + OFFSET.x, 7.0f * UNIT + OFFSET.y), ImVec2(7.0f * UNIT + OFFSET.x, 3.0f * UNIT + OFFSET.y), COLOR, 0.0f, 0, THICKNESS);
            draw_list->AddRect(ImVec2(4.0f * UNIT + OFFSET.x, 6.0f * UNIT + OFFSET.y), ImVec2(6.0f * UNIT + OFFSET.x, 4.0f * UNIT + OFFSET.y), COLOR, 0.0f, 0, THICKNESS);

            draw_list->AddLine(ImVec2(5.0f * UNIT + OFFSET.x, 2.0f * UNIT + OFFSET.y), ImVec2(5.0f * UNIT + OFFSET.x, 4.0f * UNIT + OFFSET.y), COLOR, THICKNESS);
            draw_list->AddLine(ImVec2(6.0f * UNIT + OFFSET.x, 5.0f * UNIT + OFFSET.y), ImVec2(8.0f * UNIT + OFFSET.x, 5.0f * UNIT + OFFSET.y), COLOR, THICKNESS);
            draw_list->AddLine(ImVec2(5.0f * UNIT + OFFSET.x, 6.0f * UNIT + OFFSET.y), ImVec2(5.0f * UNIT + OFFSET.x, 8.0f * UNIT + OFFSET.y), COLOR, THICKNESS);
            draw_list->AddLine(ImVec2(2.0f * UNIT + OFFSET.x, 5.0f * UNIT + OFFSET.y), ImVec2(4.0f * UNIT + OFFSET.x, 5.0f * UNIT + OFFSET.y), COLOR, THICKNESS);

            draw_list->AddText(ImVec2(2.0f * UNIT + OFFSET.x, 1.0f * UNIT + OFFSET.y), COLOR, "A");
            draw_list->AddText(ImVec2(3.0f * UNIT + OFFSET.x, 1.0f * UNIT + OFFSET.y), COLOR, "B");
            draw_list->AddText(ImVec2(4.0f * UNIT + OFFSET.x, 1.0f * UNIT + OFFSET.y), COLOR, "C");
            draw_list->AddText(ImVec2(5.0f * UNIT + OFFSET.x, 1.0f * UNIT + OFFSET.y), COLOR, "D");
            draw_list->AddText(ImVec2(6.0f * UNIT + OFFSET.x, 1.0f * UNIT + OFFSET.y), COLOR, "E");
            draw_list->AddText(ImVec2(7.0f * UNIT + OFFSET.x, 1.0f * UNIT + OFFSET.y), COLOR, "F");
            draw_list->AddText(ImVec2(8.0f * UNIT + OFFSET.x, 1.0f * UNIT + OFFSET.y), COLOR, "G");

            draw_list->AddText(ImVec2(2.0f * UNIT + OFFSET.x, 9.0f * UNIT + OFFSET.y), COLOR, "A");
            draw_list->AddText(ImVec2(3.0f * UNIT + OFFSET.x, 9.0f * UNIT + OFFSET.y), COLOR, "B");
            draw_list->AddText(ImVec2(4.0f * UNIT + OFFSET.x, 9.0f * UNIT + OFFSET.y), COLOR, "C");
            draw_list->AddText(ImVec2(5.0f * UNIT + OFFSET.x, 9.0f * UNIT + OFFSET.y), COLOR, "D");
            draw_list->AddText(ImVec2(6.0f * UNIT + OFFSET.x, 9.0f * UNIT + OFFSET.y), COLOR, "E");
            draw_list->AddText(ImVec2(7.0f * UNIT + OFFSET.x, 9.0f * UNIT + OFFSET.y), COLOR, "F");
            draw_list->AddText(ImVec2(8.0f * UNIT + OFFSET.x, 9.0f * UNIT + OFFSET.y), COLOR, "G");

            draw_list->AddText(ImVec2(9.0f * UNIT + OFFSET.x, 2.0f * UNIT + OFFSET.y), COLOR, "7");
            draw_list->AddText(ImVec2(9.0f * UNIT + OFFSET.x, 3.0f * UNIT + OFFSET.y), COLOR, "6");
            draw_list->AddText(ImVec2(9.0f * UNIT + OFFSET.x, 4.0f * UNIT + OFFSET.y), COLOR, "5");
            draw_list->AddText(ImVec2(9.0f * UNIT + OFFSET.x, 5.0f * UNIT + OFFSET.y), COLOR, "4");
            draw_list->AddText(ImVec2(9.0f * UNIT + OFFSET.x, 6.0f * UNIT + OFFSET.y), COLOR, "3");
            draw_list->AddText(ImVec2(9.0f * UNIT + OFFSET.x, 7.0f * UNIT + OFFSET.y), COLOR, "2");
            draw_list->AddText(ImVec2(9.0f * UNIT + OFFSET.x, 8.0f * UNIT + OFFSET.y), COLOR, "1");

            draw_list->AddText(ImVec2(1.0f * UNIT + OFFSET.x, 2.0f * UNIT + OFFSET.y), COLOR, "7");
            draw_list->AddText(ImVec2(1.0f * UNIT + OFFSET.x, 3.0f * UNIT + OFFSET.y), COLOR, "6");
            draw_list->AddText(ImVec2(1.0f * UNIT + OFFSET.x, 4.0f * UNIT + OFFSET.y), COLOR, "5");
            draw_list->AddText(ImVec2(1.0f * UNIT + OFFSET.x, 5.0f * UNIT + OFFSET.y), COLOR, "4");
            draw_list->AddText(ImVec2(1.0f * UNIT + OFFSET.x, 6.0f * UNIT + OFFSET.y), COLOR, "3");
            draw_list->AddText(ImVec2(1.0f * UNIT + OFFSET.x, 7.0f * UNIT + OFFSET.y), COLOR, "2");
            draw_list->AddText(ImVec2(1.0f * UNIT + OFFSET.x, 8.0f * UNIT + OFFSET.y), COLOR, "1");

            for (Idx i {0}; i < 24; i++) {
                switch (board[i]) {
                    case Piece::None:
                        break;
                    case Piece::White:
                        draw_list->AddCircleFilled(
                            ImVec2(
                                static_cast<float>(NODE_POSITIONS[i][0]) * board_unit + board_offset.x,
                                static_cast<float>(NODE_POSITIONS[i][1]) * board_unit + board_offset.y
                            ),
                            board_unit / NODE_RADIUS,
                            ImColor(235, 235, 235, 255)
                        );
                        break;
                    case Piece::Black:
                        draw_list->AddCircleFilled(
                            ImVec2(
                                static_cast<float>(NODE_POSITIONS[i][0]) * board_unit + board_offset.x,
                                static_cast<float>(NODE_POSITIONS[i][1]) * board_unit + board_offset.y
                            ),
                            board_unit / NODE_RADIUS,
                            ImColor(15, 15, 15, 255)
                        );
                        break;
                }
            }

            const float WIDTH {board_unit < 55.0f ? 2.0f : 3.0f};

            if (user_stored_index1 != NULL_INDEX) {
                const ImVec2 position {
                    static_cast<float>(NODE_POSITIONS[user_stored_index1][0]) * board_unit + board_offset.x,
                    static_cast<float>(NODE_POSITIONS[user_stored_index1][1]) * board_unit + board_offset.y
                };

                draw_list->AddCircle(position, board_unit / NODE_RADIUS + 1.0f, ImColor(240, 30, 30, 255), 0, WIDTH);
            }

            if (user_stored_index2 != NULL_INDEX) {
                const ImVec2 position {
                    static_cast<float>(NODE_POSITIONS[user_stored_index2][0]) * board_unit + board_offset.x,
                    static_cast<float>(NODE_POSITIONS[user_stored_index2][1]) * board_unit + board_offset.y
                };

                draw_list->AddCircle(position, board_unit / NODE_RADIUS + 1.0f, ImColor(30, 30, 240, 255), 0, WIDTH);
            }

            if (user_input) {
                update_user_input();
            }
        }

        ImGui::End();

        ImGui::PopStyleVar(2);
    }

    void MuhleBoard::reset(std::string_view position_string) {
        board = {};
        turn = Player::White;
        game_over = GameOver::None;
        plies = 0;
        plies_without_advancement = 0;
        positions.clear();

        user_stored_index1 = NULL_INDEX;
        user_stored_index2 = NULL_INDEX;
        user_must_take_piece = false;

        legal_moves = generate_moves();
    }

    void MuhleBoard::debug() const {
        if (ImGui::Begin("Board Internal")) {
            const char* game_over_string = nullptr;

            switch (game_over) {
                case GameOver::None:
                    game_over_string = "None";
                    break;
                case GameOver::WinnerWhite:
                    game_over_string = "WinnerWhite";
                    break;
                case GameOver::WinnerBlack:
                    game_over_string = "WinnerBlack";
                    break;
                case GameOver::TieBetweenBothPlayers:
                    game_over_string = "TieBetweenBothPlayers";
                    break;
            }

            ImGui::Text("Turn: %s", turn == Player::White ? "white" : "black");
            ImGui::Text("Game over: %s", game_over_string);
            ImGui::Text("Plies: %u", plies);
            ImGui::Text("Plies without advancement: %u", plies_without_advancement);
            ImGui::Text("Positions: %lu", positions.size());
            ImGui::Text("User stored index 1: %d", user_stored_index1);
            ImGui::Text("User stored index 2: %d", user_stored_index2);
            ImGui::Text("User must take piece: %s", user_must_take_piece ? "true" : "false");
            ImGui::Text("Legal moves: %lu", legal_moves.size());
        }

        ImGui::End();
    }

    void MuhleBoard::place(Idx place_index) {
        assert(board[place_index] == Piece::None);

        board[place_index] = static_cast<Piece>(turn);

        Move move;
        move.type = MoveType::Place;
        move.place.place_index = place_index;
        move_callback(move, turn);

        finish_turn();
        check_winner_blocking();
    }

    void MuhleBoard::place_take(Idx place_index, Idx take_index) {
        assert(board[place_index] == Piece::None);
        assert(board[take_index] != Piece::None);

        board[place_index] = static_cast<Piece>(turn);
        board[take_index] = Piece::None;

        Move move;
        move.type = MoveType::PlaceTake;
        move.place_take.place_index = place_index;
        move.place_take.take_index = take_index;
        move_callback(move, turn);

        finish_turn();
        check_winner_material();
        check_winner_blocking();
    }

    void MuhleBoard::move(Idx source_index, Idx destination_index) {
        assert(board[source_index] != Piece::None);
        assert(board[destination_index] == Piece::None);

        std::swap(board[source_index], board[destination_index]);

        Move move;
        move.type = MoveType::Move;
        move.move.source_index = source_index;
        move.move.destination_index = destination_index;
        move_callback(move, turn);

        finish_turn(false);
        check_winner_blocking();
        check_fifty_move_rule();
        check_threefold_repetition({board, turn});
    }

    void MuhleBoard::move_take(Idx source_index, Idx destination_index, Idx take_index) {
        assert(board[source_index] != Piece::None);
        assert(board[destination_index] == Piece::None);
        assert(board[take_index] != Piece::None);

        std::swap(board[source_index], board[destination_index]);
        board[take_index] = Piece::None;

        Move move;
        move.type = MoveType::MoveTake;
        move.move_take.source_index = source_index;
        move.move_take.destination_index = destination_index;
        move.move_take.take_index = take_index;
        move_callback(move, turn);

        finish_turn();
        check_winner_material();
        check_winner_blocking();
    }

    void MuhleBoard::update_user_input() {
        if (!ImGui::IsWindowFocused()) {
            return;
        }

        if (game_over != GameOver::None) {
            return;
        }

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            const ImVec2 position {ImGui::GetMousePos()};
            const Idx index {get_index(position)};

            if (index == NULL_INDEX) {
                return;
            }

            const bool phase_two {plies >= 18};

            if (phase_two) {
                if (select_piece(index)) {
                    return;
                }
            }

            if (phase_two) {
                if (user_must_take_piece) {
                    try_move_take(user_stored_index1, user_stored_index2, index);
                } else {
                    try_move(user_stored_index1, index);
                }
            } else {
                if (user_must_take_piece) {
                    try_place_take(user_stored_index2, index);
                } else {
                    try_place(index);
                }
            }
        }
    }

    bool MuhleBoard::select_piece(Idx index) {
        if (user_stored_index1 == NULL_INDEX) {
            if (board[index] == static_cast<Piece>(turn)) {
                user_stored_index1 = index;
                return true;
            }
        } else {
            if (index == user_stored_index1) {
                if (user_stored_index2 == NULL_INDEX) {
                    user_stored_index1 = NULL_INDEX;
                }

                return true;
            } else if (board[index] == static_cast<Piece>(turn)) {
                if (user_stored_index2 == NULL_INDEX) {
                    user_stored_index1 = index;
                }

                return true;
            }
        }

        return false;
    }

    void MuhleBoard::try_place(Idx place_index) {
        for (const Move& m : legal_moves) {
            if (m.type == MoveType::Place) {
                if (m.place.place_index == place_index) {
                    place(place_index);
                    return;
                }
            }
        }

        for (const Move& m : legal_moves) {
            if (m.type == MoveType::PlaceTake) {
                if (m.place_take.place_index == place_index) {
                    user_must_take_piece = true;
                    user_stored_index2 = place_index;
                    return;
                }
            }
        }
    }

    void MuhleBoard::try_place_take(Idx place_index, Idx take_index) {
        for (const Move& m : legal_moves) {
            if (m.type == MoveType::PlaceTake) {
                if (m.place_take.place_index == place_index && m.place_take.take_index == take_index) {
                    place_take(place_index, take_index);
                    return;
                }
            }
        }
    }

    void MuhleBoard::try_move(Idx source_index, Idx destination_index) {
        for (const Move& m : legal_moves) {
            if (m.type == MoveType::Move) {
                if (m.move.source_index == source_index && m.move.destination_index == destination_index) {
                    move(source_index, destination_index);
                    return;
                }
            }
        }

        for (const Move& m : legal_moves) {
            if (m.type == MoveType::MoveTake) {
                if (m.move_take.source_index == source_index && m.move_take.destination_index == destination_index) {
                    user_must_take_piece = true;
                    user_stored_index2 = destination_index;
                    return;
                }
            }
        }
    }

    void MuhleBoard::try_move_take(Idx source_index, Idx destination_index, Idx take_index) {
        for (const Move& m : legal_moves) {
            if (m.type == MoveType::MoveTake) {
                const bool match {
                    m.move_take.source_index == source_index &&
                    m.move_take.destination_index == destination_index &&
                    m.move_take.take_index == take_index
                };

                if (match) {
                    move_take(source_index, destination_index, take_index);
                    return;
                }
            }
        }
    }

    void MuhleBoard::finish_turn(bool advancement) {
        if (turn == Player::White) {
            turn = Player::Black;
        } else {
            turn = Player::White;
        }

        plies++;
        legal_moves = generate_moves();

        if (advancement) {
            plies_without_advancement = 0;
            positions.clear();
        } else {
            plies_without_advancement++;
        }

        positions.push_back({board, turn});

        user_stored_index1 = NULL_INDEX;
        user_stored_index2 = NULL_INDEX;
        user_must_take_piece = false;
    }

    void MuhleBoard::check_winner_material() {
        if (game_over != GameOver::None) {
            return;
        }

        if (count_pieces(turn) < 3) {
            game_over = static_cast<GameOver>(opponent(turn));
        }
    }

    void MuhleBoard::check_winner_blocking() {
        if (game_over != GameOver::None) {
            return;
        }

        if (legal_moves.empty()) {
            game_over = static_cast<GameOver>(opponent(turn));
        }
    }

    void MuhleBoard::check_fifty_move_rule() {
        if (game_over != GameOver::None) {
            return;
        }

        if (plies_without_advancement == 100) {
            game_over = GameOver::TieBetweenBothPlayers;
        }
    }

    void MuhleBoard::check_threefold_repetition(const Position& position) {
        if (game_over != GameOver::None) {
            return;
        }

        unsigned int repetitions {1};

        for (auto iter {positions.begin()}; iter != std::prev(positions.end()); iter++) {
            if (*iter == position) {
                if (++repetitions == 3) {
                    game_over = GameOver::TieBetweenBothPlayers;
                    return;
                }
            }
        }
    }

    std::vector<Move> MuhleBoard::generate_moves() const {
        std::vector<Move> moves;
        Board local_board {board};

        if (plies < 18) {
            generate_moves_phase1(local_board, moves, turn);
        } else {
            if (count_pieces(turn) == 3) {
                generate_moves_phase3(local_board, moves, turn);
            } else {
                generate_moves_phase2(local_board, moves, turn);
            }
        }

        return moves;
    }

    void MuhleBoard::generate_moves_phase1(Board& board, std::vector<Move>& moves, Player player) {
        for (Idx i {0}; i < 24; i++) {
            if (board[i] != Piece::None) {
                continue;
            }

            make_place_move(board, player, i);

            if (is_mill(board, player, i)) {
                const Player opponent_player {opponent(player)};
                const bool all_in_mills {all_pieces_in_mills(board, opponent_player)};

                for (Idx j {0}; j < 24; j++) {
                    if (board[j] != static_cast<Piece>(opponent_player)) {
                        continue;
                    }

                    if (is_mill(board, opponent_player, j) && !all_in_mills) {
                        continue;
                    }

                    moves.push_back(create_place_take(i, j));
                }
            } else {
                moves.push_back(create_place(i));
            }

            unmake_place_move(board, i);
        }
    }

    void MuhleBoard::generate_moves_phase2(Board& board, std::vector<Move>& moves, Player player) {
        for (Idx i {0}; i < 24; i++) {
            if (board[i] != static_cast<Piece>(player)) {
                continue;
            }

            const auto free_positions {neighbor_free_positions(board, i)};

            for (Idx j {0}; j < static_cast<Idx>(free_positions.size()); j++) {
                make_move_move(board, i, free_positions[j]);

                if (is_mill(board, player, free_positions[j])) {
                    const Player opponent_player {opponent(player)};
                    const bool all_in_mills {all_pieces_in_mills(board, opponent_player)};

                    for (Idx k {0}; k < 24; k++) {
                        if (board[k] != static_cast<Piece>(opponent_player)) {
                            continue;
                        }

                        if (is_mill(board, opponent_player, k) && !all_in_mills) {
                            continue;
                        }

                        moves.push_back(create_move_take(i, free_positions[j], k));
                    }
                } else {
                    moves.push_back(create_move(i, free_positions[j]));
                }

                unmake_move_move(board, i, free_positions[j]);
            }
        }
    }

    void MuhleBoard::generate_moves_phase3(Board& board, std::vector<Move>& moves, Player player) {
        for (Idx i {0}; i < 24; i++) {
            if (board[i] != static_cast<Piece>(player)) {
                continue;
            }

            for (Idx j {0}; j < 24; j++) {
                if (board[j] != Piece::None) {
                    continue;
                }

                make_move_move(board, i, j);

                if (is_mill(board, player, j)) {
                    const Player opponent_player {opponent(player)};
                    const bool all_in_mills {all_pieces_in_mills(board, opponent_player)};

                    for (Idx k {0}; k < 24; k++) {
                        if (board[k] != static_cast<Piece>(opponent_player)) {
                            continue;
                        }

                        if (is_mill(board, opponent_player, k) && !all_in_mills) {
                            continue;
                        }

                        moves.push_back(create_move_take(i, j, k));
                    }
                } else {
                    moves.push_back(create_move(i, j));
                }

                unmake_move_move(board, i, j);
            }
        }
    }

    void MuhleBoard::make_place_move(Board& board, Player player, Idx place_index) {
        assert(board[place_index] == Piece::None);

        board[place_index] = static_cast<Piece>(player);
    }

    void MuhleBoard::unmake_place_move(Board& board, Idx place_index) {
        assert(board[place_index] != Piece::None);

        board[place_index] = Piece::None;
    }

    void MuhleBoard::make_move_move(Board& board, Idx source_index, Idx destination_index) {
        assert(board[source_index] != Piece::None);
        assert(board[destination_index] == Piece::None);

        std::swap(board[source_index], board[destination_index]);
    }

    void MuhleBoard::unmake_move_move(Board& board, Idx source_index, Idx destination_index) {
        assert(board[source_index] == Piece::None);
        assert(board[destination_index] != Piece::None);

        std::swap(board[source_index], board[destination_index]);
    }

#ifdef __GNUG__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wparentheses"
#endif

#define IS_PC(const_index) (board[const_index] == piece)

    bool MuhleBoard::is_mill(const Board& board, Player player, Idx index) {
        const Piece piece {static_cast<Piece>(player)};

        switch (index) {
            case 0:
                if (IS_PC(1) && IS_PC(2) || IS_PC(9) && IS_PC(21))
                    return true;
                break;
            case 1:
                if (IS_PC(0) && IS_PC(2) || IS_PC(4) && IS_PC(7))
                    return true;
                break;
            case 2:
                if (IS_PC(0) && IS_PC(1) || IS_PC(14) && IS_PC(23))
                    return true;
                break;
            case 3:
                if (IS_PC(4) && IS_PC(5) || IS_PC(10) && IS_PC(18))
                    return true;
                break;
            case 4:
                if (IS_PC(3) && IS_PC(5) || IS_PC(1) && IS_PC(7))
                    return true;
                break;
            case 5:
                if (IS_PC(3) && IS_PC(4) || IS_PC(13) && IS_PC(20))
                    return true;
                break;
            case 6:
                if (IS_PC(7) && IS_PC(8) || IS_PC(11) && IS_PC(15))
                    return true;
                break;
            case 7:
                if (IS_PC(6) && IS_PC(8) || IS_PC(1) && IS_PC(4))
                    return true;
                break;
            case 8:
                if (IS_PC(6) && IS_PC(7) || IS_PC(12) && IS_PC(17))
                    return true;
                break;
            case 9:
                if (IS_PC(0) && IS_PC(21) || IS_PC(10) && IS_PC(11))
                    return true;
                break;
            case 10:
                if (IS_PC(9) && IS_PC(11) || IS_PC(3) && IS_PC(18))
                    return true;
                break;
            case 11:
                if (IS_PC(9) && IS_PC(10) || IS_PC(6) && IS_PC(15))
                    return true;
                break;
            case 12:
                if (IS_PC(13) && IS_PC(14) || IS_PC(8) && IS_PC(17))
                    return true;
                break;
            case 13:
                if (IS_PC(12) && IS_PC(14) || IS_PC(5) && IS_PC(20))
                    return true;
                break;
            case 14:
                if (IS_PC(12) && IS_PC(13) || IS_PC(2) && IS_PC(23))
                    return true;
                break;
            case 15:
                if (IS_PC(16) && IS_PC(17) || IS_PC(6) && IS_PC(11))
                    return true;
                break;
            case 16:
                if (IS_PC(15) && IS_PC(17) || IS_PC(19) && IS_PC(22))
                    return true;
                break;
            case 17:
                if (IS_PC(15) && IS_PC(16) || IS_PC(8) && IS_PC(12))
                    return true;
                break;
            case 18:
                if (IS_PC(19) && IS_PC(20) || IS_PC(3) && IS_PC(10))
                    return true;
                break;
            case 19:
                if (IS_PC(18) && IS_PC(20) || IS_PC(16) && IS_PC(22))
                    return true;
                break;
            case 20:
                if (IS_PC(18) && IS_PC(19) || IS_PC(5) && IS_PC(13))
                    return true;
                break;
            case 21:
                if (IS_PC(22) && IS_PC(23) || IS_PC(0) && IS_PC(9))
                    return true;
                break;
            case 22:
                if (IS_PC(21) && IS_PC(23) || IS_PC(16) && IS_PC(19))
                    return true;
                break;
            case 23:
                if (IS_PC(21) && IS_PC(22) || IS_PC(2) && IS_PC(14))
                    return true;
                break;
        }

        return false;
    }

#ifdef __GNUG__
    #pragma GCC diagnostic pop
#endif

    bool MuhleBoard::all_pieces_in_mills(const Board& board, Player player) {
        for (Idx i {0}; i < 24; i++) {
            if (board[i] != static_cast<Piece>(player)) {
                continue;
            }

            if (!is_mill(board, player, i)) {
                return false;
            }
        }

        return true;
    }

#define IS_FREE_CHECK(const_index) \
    if (board[const_index] == Piece::None) { \
        result.push_back(const_index); \
    }

    std::vector<Idx> MuhleBoard::neighbor_free_positions(const Board& board, Idx index) {
        std::vector<Idx> result;
        result.reserve(4);

        switch (index) {
            case 0:
                IS_FREE_CHECK(1)
                IS_FREE_CHECK(9)
                break;
            case 1:
                IS_FREE_CHECK(0)
                IS_FREE_CHECK(2)
                IS_FREE_CHECK(4)
                break;
            case 2:
                IS_FREE_CHECK(1)
                IS_FREE_CHECK(14)
                break;
            case 3:
                IS_FREE_CHECK(4)
                IS_FREE_CHECK(10)
                break;
            case 4:
                IS_FREE_CHECK(1)
                IS_FREE_CHECK(3)
                IS_FREE_CHECK(5)
                IS_FREE_CHECK(7)
                break;
            case 5:
                IS_FREE_CHECK(4)
                IS_FREE_CHECK(13)
                break;
            case 6:
                IS_FREE_CHECK(7)
                IS_FREE_CHECK(11)
                break;
            case 7:
                IS_FREE_CHECK(4)
                IS_FREE_CHECK(6)
                IS_FREE_CHECK(8)
                break;
            case 8:
                IS_FREE_CHECK(7)
                IS_FREE_CHECK(12)
                break;
            case 9:
                IS_FREE_CHECK(0)
                IS_FREE_CHECK(10)
                IS_FREE_CHECK(21)
                break;
            case 10:
                IS_FREE_CHECK(3)
                IS_FREE_CHECK(9)
                IS_FREE_CHECK(11)
                IS_FREE_CHECK(18)
                break;
            case 11:
                IS_FREE_CHECK(6)
                IS_FREE_CHECK(10)
                IS_FREE_CHECK(15)
                break;
            case 12:
                IS_FREE_CHECK(8)
                IS_FREE_CHECK(13)
                IS_FREE_CHECK(17)
                break;
            case 13:
                IS_FREE_CHECK(5)
                IS_FREE_CHECK(12)
                IS_FREE_CHECK(14)
                IS_FREE_CHECK(20)
                break;
            case 14:
                IS_FREE_CHECK(2)
                IS_FREE_CHECK(13)
                IS_FREE_CHECK(23)
                break;
            case 15:
                IS_FREE_CHECK(11)
                IS_FREE_CHECK(16)
                break;
            case 16:
                IS_FREE_CHECK(15)
                IS_FREE_CHECK(17)
                IS_FREE_CHECK(19)
                break;
            case 17:
                IS_FREE_CHECK(12)
                IS_FREE_CHECK(16)
                break;
            case 18:
                IS_FREE_CHECK(10)
                IS_FREE_CHECK(19)
                break;
            case 19:
                IS_FREE_CHECK(16)
                IS_FREE_CHECK(18)
                IS_FREE_CHECK(20)
                IS_FREE_CHECK(22)
                break;
            case 20:
                IS_FREE_CHECK(13)
                IS_FREE_CHECK(19)
                break;
            case 21:
                IS_FREE_CHECK(9)
                IS_FREE_CHECK(22)
                break;
            case 22:
                IS_FREE_CHECK(19)
                IS_FREE_CHECK(21)
                IS_FREE_CHECK(23)
                break;
            case 23:
                IS_FREE_CHECK(14)
                IS_FREE_CHECK(22)
                break;
        }

        return result;
    }

    Move MuhleBoard::create_place(Idx place_index) {
        Move move;
        move.type = MoveType::Place;
        move.place.place_index = place_index;

        return move;
    }

    Move MuhleBoard::create_place_take(Idx place_index, Idx take_index) {
        Move move;
        move.type = MoveType::PlaceTake;
        move.place_take.place_index = place_index;
        move.place_take.take_index = take_index;

        return move;
    }

    Move MuhleBoard::create_move(Idx source_index, Idx destination_index) {
        Move move;
        move.type = MoveType::Move;
        move.move.source_index = source_index;
        move.move.destination_index = destination_index;

        return move;
    }

    Move MuhleBoard::create_move_take(Idx source_index, Idx destination_index, Idx take_index) {
        Move move;
        move.type = MoveType::MoveTake;
        move.move_take.source_index = source_index;
        move.move_take.destination_index = destination_index;
        move.move_take.take_index = take_index;

        return move;
    }

    Idx MuhleBoard::get_index(ImVec2 position) const {
        for (Idx i {0}; i < 24; i++) {
            const ImVec2 node {
                static_cast<float>(NODE_POSITIONS[i][0]) * board_unit + board_offset.x,
                static_cast<float>(NODE_POSITIONS[i][1]) * board_unit + board_offset.y
            };

            if (point_in_circle(position, node, board_unit / NODE_RADIUS)) {
                return i;
            }
        }

        return NULL_INDEX;
    }

    unsigned int MuhleBoard::count_pieces(Player player) const {
        unsigned int result {0};

        for (const Piece piece : board) {
            result += static_cast<unsigned int>(piece == static_cast<Piece>(player));
        }

        return result;
    }

    Player MuhleBoard::opponent(Player player) {
        if (player == Player::White) {
            return Player::Black;
        } else {
            return Player::White;
        }
    }

    bool MuhleBoard::point_in_circle(ImVec2 point, ImVec2 circle, float radius) {
        const ImVec2 subtracted {circle.x - point.x, circle.y - point.y};
        const float length {std::pow(subtracted.x * subtracted.x + subtracted.y * subtracted.y, 0.5f)};

        return length < radius;
    }

    Move move_from_string(std::string_view string) {
        Move result {};

        switch (string[0]) {
            case 'P': {
                const Idx place_index {index_from_string(string.substr(1, 2))};

                if (string.size() > 3) {
                    const Idx take_index {index_from_string(string.substr(4, 2))};

                    result.type = MoveType::PlaceTake;
                    result.place_take.place_index = place_index;
                    result.place_take.take_index = take_index;
                } else {
                    result.type = MoveType::Place;
                    result.place.place_index = place_index;
                }

                return result;
            }
            case 'M': {
                const Idx source_index {index_from_string(string.substr(1, 2))};
                const Idx destination_index {index_from_string(string.substr(4, 2))};

                if (string.size() > 6) {
                    const Idx take_index {index_from_string(string.substr(7, 2))};

                    result.type = MoveType::MoveTake;
                    result.move_take.source_index = source_index;
                    result.move_take.destination_index = destination_index;
                    result.move_take.take_index = take_index;
                } else {
                    result.type = MoveType::Move;
                    result.move.source_index = source_index;
                    result.move.destination_index = destination_index;
                }

                return result;
            }
        }

        assert(false);

        return {};
    }

    std::string move_to_string(const Move& move) {
        std::string result;

        switch (move.type) {
            case MoveType::Place:
                result += 'P';
                result += string_from_index(move.place.place_index);
                break;
            case MoveType::PlaceTake:
                result += 'P';
                result += string_from_index(move.place_take.place_index);
                result += 'T';
                result += string_from_index(move.place_take.take_index);
                break;
            case MoveType::Move:
                result += 'M';
                result += string_from_index(move.move.source_index);
                result += '-';
                result += string_from_index(move.move.destination_index);
                break;
            case MoveType::MoveTake:
                result += 'M';
                result += string_from_index(move.move_take.source_index);
                result += '-';
                result += string_from_index(move.move_take.destination_index);
                result += 'T';
                result += string_from_index(move.move_take.take_index);
                break;
        }

        return result;
    }
}
