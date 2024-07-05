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

    MuhleBoard::MuhleBoard() {
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
                    case Node::Empty:
                        break;
                    case Node::White:
                        draw_list->AddCircleFilled(
                            ImVec2(
                                static_cast<float>(NODE_POSITIONS[i][0]) * board_unit + board_offset.x,
                                static_cast<float>(NODE_POSITIONS[i][1]) * board_unit + board_offset.y
                            ),
                            board_unit / NODE_RADIUS,
                            ImColor(235, 235, 235, 255)
                        );
                        break;
                    case Node::Black:
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

            if (user_stored_index1 != NULL_INDEX) {
                const ImVec2 position {
                    static_cast<float>(NODE_POSITIONS[user_stored_index1][0]) * board_unit + board_offset.x,
                    static_cast<float>(NODE_POSITIONS[user_stored_index1][1]) * board_unit + board_offset.y
                };
                const float WIDTH {board_unit < 55.0f ? 2.0f : 3.0f};

                draw_list->AddCircle(position, board_unit / NODE_RADIUS + 1.0f, ImColor(240, 30, 30, 255), 0, WIDTH);
            }

            if (user_stored_index2 != NULL_INDEX) {
                const ImVec2 position {
                    static_cast<float>(NODE_POSITIONS[user_stored_index2][0]) * board_unit + board_offset.x,
                    static_cast<float>(NODE_POSITIONS[user_stored_index2][1]) * board_unit + board_offset.y
                };
                const float WIDTH {board_unit < 55.0f ? 2.0f : 3.0f};

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
        assert(board[place_index] == Node::Empty);

        board[place_index] = static_cast<Node>(turn);

        finish_turn();
        check_winner_blocking();
    }

    void MuhleBoard::place_take(Idx place_index, Idx take_index) {
        assert(board[place_index] == Node::Empty);
        assert(board[take_index] != Node::Empty);

        board[place_index] = static_cast<Node>(turn);
        board[take_index] = Node::Empty;

        finish_turn();
        check_winner_material();
        check_winner_blocking();
    }

    void MuhleBoard::move(Idx source_index, Idx destination_index) {
        assert(board[source_index] != Node::Empty);
        assert(board[destination_index] == Node::Empty);

        std::swap(board[source_index], board[destination_index]);

        finish_turn(false);
        check_winner_blocking();
        check_fifty_move_rule();
        check_threefold_repetition({board, turn});
    }

    void MuhleBoard::move_take(Idx source_index, Idx destination_index, Idx take_index) {
        assert(board[source_index] != Node::Empty);
        assert(board[destination_index] == Node::Empty);
        assert(board[take_index] != Node::Empty);

        std::swap(board[source_index], board[destination_index]);
        board[take_index] = Node::Empty;

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
            if (board[index] == static_cast<Node>(turn)) {
                user_stored_index1 = index;
                return true;
            }
        } else {
            if (user_stored_index1 == index) {
                user_stored_index1 = NULL_INDEX;
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
            generate_moves_phase1(turn, local_board, moves);
        } else {
            if (count_pieces(turn) == 3) {
                generate_moves_phase3(turn, local_board, moves);
            } else {
                generate_moves_phase2(turn, local_board, moves);
            }
        }

        return moves;
    }

    void MuhleBoard::generate_moves_phase1(Player player, Board& board, std::vector<Move>& moves) {
        for (Idx i {0}; i < 24; i++) {
            if (board[i] != Node::Empty) {
                continue;
            }

            make_place_move(player, i, board);

            if (is_mill(player, i, board)) {
                const Player opponent_player {opponent(player)};
                const bool all_in_mills {all_pieces_in_mills(opponent_player, board)};

                for (Idx j {0}; j < 24; j++) {
                    if (board[j] != static_cast<Node>(opponent_player)) {
                        continue;
                    }

                    if (is_mill(opponent_player, j, board) && !all_in_mills) {
                        continue;
                    }

                    moves.push_back(create_place_take(i, j));
                }
            } else {
                moves.push_back(create_place(i));
            }

            unmake_place_move(i, board);
        }
    }

    void MuhleBoard::generate_moves_phase2(Player player, Board& board, std::vector<Move>& moves) {
        for (Idx i {0}; i < 24; i++) {
            if (board[i] != static_cast<Node>(player)) {
                continue;
            }

            const auto free_positions {neighbor_free_positions(i, board)};

            for (Idx j {0}; j < static_cast<Idx>(free_positions.size()); j++) {
                make_move_move(i, free_positions[j], board);

                if (is_mill(player, free_positions[j], board)) {
                    const Player opponent_player {opponent(player)};
                    const bool all_in_mills {all_pieces_in_mills(opponent_player, board)};

                    for (Idx k {0}; k < 24; k++) {
                        if (board[k] != static_cast<Node>(opponent_player)) {
                            continue;
                        }

                        if (is_mill(opponent_player, k, board) && !all_in_mills) {
                            continue;
                        }

                        moves.push_back(create_move_take(i, free_positions[j], k));
                    }
                } else {
                    moves.push_back(create_move(i, free_positions[j]));
                }

                unmake_move_move(i, free_positions[j], board);
            }
        }
    }

    void MuhleBoard::generate_moves_phase3(Player player, Board& board, std::vector<Move>& moves) {
        for (Idx i {0}; i < 24; i++) {
            if (board[i] != static_cast<Node>(player)) {
                continue;
            }

            for (Idx j {0}; j < 24; j++) {
                if (board[j] != Node::Empty) {
                    continue;
                }

                make_move_move(i, j, board);

                if (is_mill(player, j, board)) {
                    const Player opponent_player {opponent(player)};
                    const bool all_in_mills {all_pieces_in_mills(opponent_player, board)};

                    for (Idx k {0}; k < 24; k++) {
                        if (board[k] != static_cast<Node>(opponent_player)) {
                            continue;
                        }

                        if (is_mill(opponent_player, k, board) && !all_in_mills) {
                            continue;
                        }

                        moves.push_back(create_move_take(i, j, k));
                    }
                } else {
                    moves.push_back(create_move(i, j));
                }

                unmake_move_move(i, j, board);
            }
        }
    }

    void MuhleBoard::make_place_move(Player player, Idx place_index, Board& local_board) {
        assert(local_board[place_index] == Node::Empty);

        local_board[place_index] = static_cast<Node>(player);
    }

    void MuhleBoard::unmake_place_move(Idx place_index, Board& local_board) {
        assert(local_board[place_index] != Node::Empty);

        local_board[place_index] = Node::Empty;
    }

    void MuhleBoard::make_move_move(Idx source_index, Idx destination_index, Board& local_board) {
        assert(local_board[source_index] != Node::Empty);
        assert(local_board[destination_index] == Node::Empty);

        std::swap(local_board[source_index], local_board[destination_index]);
    }

    void MuhleBoard::unmake_move_move(Idx source_index, Idx destination_index, Board& local_board) {
        assert(local_board[source_index] == Node::Empty);
        assert(local_board[destination_index] != Node::Empty);

        std::swap(local_board[source_index], local_board[destination_index]);
    }

#define IS_PC(const_index) (board[const_index] == node)

    bool MuhleBoard::is_mill(Player player, Idx index, const Board& board) {
        const Node node {static_cast<Node>(player)};

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

    bool MuhleBoard::all_pieces_in_mills(Player player, const Board& board) {
        for (Idx i {0}; i < 24; i++) {
            if (board[i] != static_cast<Node>(player)) {
                continue;
            }

            if (!is_mill(player, i, board)) {
                return false;
            }
        }

        return true;
    }

#define IS_FREE_CHECK(const_index) \
    if (board[const_index] == Node::Empty) { \
        result.push_back(const_index); \
    }

    std::vector<Idx> MuhleBoard::neighbor_free_positions(Idx index, const Board& board) {
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

        for (Node node : board) {
            result += static_cast<unsigned int>(node == static_cast<Node>(player));
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
        const ImVec2 subtracted {ImVec2(circle.x - point.x, circle.y - point.y)};
        const float length {std::pow(subtracted.x * subtracted.x + subtracted.y * subtracted.y, 0.5f)};

        return length < radius;
    }
}
