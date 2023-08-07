#include <gui_base.hpp>
#include <glm/glm.hpp>
#include <muhle_intelligence/muhle_intelligence.hpp>
#include <muhle_intelligence/miscellaneous.hpp>

#include "modes/play.hpp"
#include "modes/common.hpp"

void TesterModePlay::update(muhle::MuhleIntelligence* muhle, muhle::Result& muhle_result, float board_unit, glm::vec2 board_offset) {
    game_play.update_nodes_positions(board_unit, board_offset);

    switch (play_state) {
        case PlayState::NextTurn:
            if (game_play.phase != GamePhase::GameOver) {
                switch (game_play.turn) {
                    case Player::White:
                        switch (white) {
                            case PlayerHuman:
                                play_state = PlayState::HumanThinking;
                                break;
                            case PlayerComputer:
                                play_state = PlayState::ComputerBegin;
                                break;
                        }

                        break;
                    case Player::Black:
                        switch (black) {
                            case PlayerHuman:
                                play_state = PlayState::HumanThinking;
                                break;
                            case PlayerComputer:
                                play_state = PlayState::ComputerBegin;
                                break;
                        }

                        break;
                }
            }

            break;
        case PlayState::HumanThinking:
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                const ImVec2 position = ImGui::GetMousePos();
                game_play.user_click(glm::vec2(position.x, position.y));

                play_state = PlayState::NextTurn;
            }

            break;
        case PlayState::ComputerBegin: {
            if (muhle != nullptr) {
                const auto game_position = game_play.get_position();

                muhle::Position position;
                for (size_t i = 0; i < game_position.size(); i++) {
                    position.pieces[i] = static_cast<muhle::Piece>(game_position[i]);
                }
                position.plies = game_play.plies;

                muhle->search(
                    position,
                    game_play.turn == Player::White ? muhle::Player::White : muhle::Player::Black,
                    muhle_result
                );

                play_state = PlayState::ComputerThinking;
            }

            break;
        }
        case PlayState::ComputerThinking:
            if (muhle_result.done) {
                muhle::print_result_statistics(muhle_result);

                switch (muhle_result.result.type) {
                    case muhle::MoveType::Place:
                        game_play.place_piece(muhle_result.result.place.node_index);
                        break;
                    case muhle::MoveType::Move:
                        game_play.move_piece(
                            muhle_result.result.move.node_source_index,
                            muhle_result.result.move.node_destination_index
                        );
                        break;
                    case muhle::MoveType::PlaceTake:
                        game_play.place_piece(muhle_result.result.place_take.node_index);
                        game_play.take_piece(muhle_result.result.place_take.node_take_index);
                        break;
                    case muhle::MoveType::MoveTake:
                        game_play.move_piece(
                            muhle_result.result.move_take.node_source_index,
                            muhle_result.result.move_take.node_destination_index
                        );
                        game_play.take_piece(muhle_result.result.move_take.node_take_index);
                        break;
                }

                computer_move_history.push_back(muhle_result);

                play_state = PlayState::ComputerEnd;
            }

            break;
        case PlayState::ComputerEnd:
            play_state = PlayState::NextTurn;

            break;
    }
}

void TesterModePlay::draw(ImDrawList* draw_list) {
    for (const Node& node : game_play.nodes) {
        if (!node.piece.has_value()) {
            continue;
        }

        const Piece& piece = node.piece.value();

        draw_piece(draw_list, piece.position.x, piece.position.y, piece.player);
    }

    if (game_play.selected_piece_index != INVALID_INDEX) {
        const Piece& piece = game_play.nodes[game_play.selected_piece_index].piece.value();

        draw_list->AddCircle(
            ImVec2(piece.position.x, piece.position.y),
            NODE_RADIUS + 1.0f,
            ImColor(255, 30, 30, 255),
            0,
            2.0f
        );
    }
}

void TesterModePlay::ui() {
    vertical_spacing();

    if (ImGui::Button("Stop AI")) {

    }

    if (ImGui::Button("Reset")) {
        reset();
    }

    vertical_spacing();

    ImGui::Text("White"); ImGui::SameLine();
    ImGui::RadioButton("Human##w", &white, PlayerHuman); ImGui::SameLine();
    ImGui::RadioButton("Computer##w", &white, PlayerComputer);

    ImGui::Text("Black"); ImGui::SameLine();
    ImGui::RadioButton("Human##b", &black, PlayerHuman); ImGui::SameLine();
    ImGui::RadioButton("Computer##b", &black, PlayerComputer);

    vertical_spacing();

    ImGui::Separator();

    vertical_spacing();

    ImGui::Text("State: %d", static_cast<int>(play_state));
    ImGui::Text("White pieces on board: %u", game_play.white_pieces_on_board);
    ImGui::Text("White pieces outside: %u", game_play.white_pieces_outside);
    ImGui::Text("Black pieces on board: %u", game_play.black_pieces_on_board);
    ImGui::Text("Black pieces outside: %u", game_play.black_pieces_outside);
    ImGui::Text("Turn: %s", game_play.turn == Player::White ? "white" : "black");
    ImGui::Text("Phase: %d", static_cast<int>(game_play.phase));
    ImGui::Text("Ending: %d", static_cast<int>(game_play.ending));
    ImGui::Text("Plies: %u", game_play.plies);
    ImGui::Text("Selected piece: %d", game_play.selected_piece_index);
    ImGui::Text("Can jump: white %d, black %d", game_play.can_jump[0], game_play.can_jump[1]);
    ImGui::Text("Must take piece: %d", game_play.must_take_piece);
    ImGui::Text("Plies without mills: %u", game_play.plies_without_mills);

    vertical_spacing();

    ImGui::Separator();

    vertical_spacing();

    ImGui::BeginChild("Moves");

    for (const muhle::Result& result : computer_move_history) {
        ImGui::Text("%s", muhle::move_to_string(result.result, result.player).c_str());
        ImGui::Spacing();
    }

    ImGui::EndChild();
}

void TesterModePlay::setup() {
    game_play.setup([this]() {
        this->change_turn();
    });
}

void TesterModePlay::reset() {
    // TODO stop everything

    game_play = {};
    game_play.setup([this]() {
        this->change_turn();
    });

    play_state = PlayState::NextTurn;

    computer_move_history.clear();
}

void TesterModePlay::change_turn() {
    switch (play_state) {
        case PlayState::HumanThinking:
            play_state = PlayState::ComputerBegin;
            break;
        case PlayState::ComputerEnd:
            play_state = PlayState::HumanThinking;
            break;
        default:
            break;
    }
}
