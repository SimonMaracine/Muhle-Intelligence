#include <vector>
#include <utility>
#include <iostream>
#include <cstddef>

#include <gui_base.hpp>
#include <glm/glm.hpp>
#include <muhle_intelligence/muhle_intelligence.hpp>
#include <muhle_intelligence/definitions.hpp>
#include <muhle_intelligence/miscellaneous.hpp>

#include "modes/play.hpp"
#include "modes/common.hpp"

void TesterModePlay::update(muhle::MuhleIntelligence* muhle, muhle::Result& muhle_result, float board_unit, glm::vec2 board_offset) {
    game_play.update_nodes_positions(board_unit, board_offset);

    switch (state) {
        case PlayState::NextTurn:
            if (game_play.phase != GamePhase::GameOver) {
                switch (game_play.turn) {
                    case Player::White:
                        switch (white) {
                            case PlayerHuman:
                                state = PlayState::HumanThinking;
                                break;
                            case PlayerComputer:
                                state = PlayState::ComputerBegin;
                                break;
                        }

                        break;
                    case Player::Black:
                        switch (black) {
                            case PlayerHuman:
                                state = PlayState::HumanThinking;
                                break;
                            case PlayerComputer:
                                state = PlayState::ComputerBegin;
                                break;
                        }

                        break;
                }
            }

            break;
        case PlayState::HumanThinking:
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                const ImVec2 position = ImGui::GetMousePos();
                game_play.user_action(glm::vec2(position.x, position.y));

                state = PlayState::NextTurn;
            }

            break;
        case PlayState::ComputerBegin: {
            const muhle::Position position = game_play.get_position();

            muhle->search(
                position,
                game_play.turn == Player::White ? muhle::Player::White : muhle::Player::Black,
                muhle_result
            );

            state = PlayState::ComputerThinking;

            break;
        }
        case PlayState::ComputerThinking:
            if (muhle_result.done) {
                muhle::print_result_statistics(muhle_result, std::cout);

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

                state = PlayState::NextTurn;
            }

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
    ImGui::Columns(2);

    ImGui::SetColumnWidth(0, 300);
    ImGui::BeginChild("Left");

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

    const char* state_str = nullptr;

    switch (state) {
        case PlayState::NextTurn:
            state_str = "NextTurn";
            break;
        case PlayState::HumanThinking:
            state_str = "HumanThinking";
            break;
        case PlayState::ComputerBegin:
            state_str = "ComputerBegin";
            break;
        case PlayState::ComputerThinking:
            state_str = "ComputerThinking";
            break;
    }

    ImGui::Text("State: %s", state_str);
    ImGui::Text("White pieces on board: %u", game_play.white_pieces_on_board);
    ImGui::Text("White pieces outside: %u", game_play.white_pieces_outside);
    ImGui::Text("Black pieces on board: %u", game_play.black_pieces_on_board);
    ImGui::Text("Black pieces outside: %u", game_play.black_pieces_outside);
    ImGui::Text("Turn: %s", game_play.player_to_string());
    ImGui::Text("Phase: %s", game_play.phase_to_string());
    ImGui::Text("Ending: %s", game_play.ending_to_string());
    ImGui::Text("Plies: %u", game_play.plies);
    ImGui::Text("Selected piece: %d", game_play.selected_piece_index);
    ImGui::Text("Can jump: white %d, black %d", game_play.can_jump[0], game_play.can_jump[1]);
    ImGui::Text("Must take piece: %d", game_play.must_take_piece);
    ImGui::Text("Plies without mills: %u", game_play.plies_without_mills);

    ImGui::EndChild();

    ImGui::NextColumn();

    ImGui::SetColumnWidth(1, 200);
    ImGui::BeginChild("Right", {}, false, ImGuiWindowFlags_AlwaysAutoResize);

    for (std::size_t i = 0; const auto& [move, player] : move_history) {
        ImGui::Text("%lu. %s", i++, muhle::move_to_string(move, player).c_str());
        ImGui::Spacing();
    }

    ImGui::EndChild();

    ImGui::Columns(1);
}

void TesterModePlay::setup() {
    game_play.setup([this](muhle::Move move, muhle::Player player) {
        move_history.push_back(std::make_tuple(move, player));
    });
}

void TesterModePlay::reset() {
    // TODO stop everything

    game_play = {};
    game_play.setup([this](muhle::Move move, muhle::Player player) {
        move_history.push_back(std::make_tuple(move, player));
    });

    state = PlayState::NextTurn;

    move_history.clear();
}
