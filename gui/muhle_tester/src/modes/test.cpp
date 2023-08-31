#include <iostream>

#include <gui_base.hpp>
#include <glm/glm.hpp>
#include <muhle_intelligence/muhle_intelligence.hpp>
#include <muhle_intelligence/definitions.hpp>
#include <muhle_intelligence/miscellaneous.hpp>

#include "modes/test.hpp"
#include "modes/common.hpp"

void TesterModeTest::update(muhle::MuhleIntelligence* muhle, muhle::Result& muhle_result, float board_unit, glm::vec2 board_offset) {
    game_test.update_nodes_positions(board_unit, board_offset);

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        const ImVec2 position = ImGui::GetMousePos();
        const Player player = piece == PieceWhite ? Player::White : Player::Black;

        game_test.user_action(glm::vec2(position.x, position.y), GameTest::MouseButton::Left, player);
    }

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
        const ImVec2 position = ImGui::GetMousePos();

        game_test.user_action(glm::vec2(position.x, position.y), GameTest::MouseButton::Right);
    }

    switch (test_state) {
        case TestState::None:
            break;
        case TestState::ComputerBegin: {
            result_text = "[Computing...]";
            statistics_text = "";

            const muhle::Position position = game_test.get_position();

            muhle->search(
                position,
                game_test.turn == Player::White ? muhle::Player::White : muhle::Player::Black,
                muhle_result
            );

            test_state = TestState::ComputerThinking;

            break;
        }
        case TestState::ComputerThinking: {
            if (muhle_result.done) {
                muhle::print_result_statistics(muhle_result, std::cout);

                result_text = muhle::move_to_string(muhle_result.result, muhle_result.player);
                muhle::print_result_statistics(muhle_result, statistics_text);

                test_state = TestState::None;
            }

            break;
        }
    }
}

void TesterModeTest::draw(ImDrawList* draw_list) {
    for (const Node& node : game_test.nodes) {
        if (!node.piece.has_value()) {
            continue;
        }

        const Piece& piece = node.piece.value();

        draw_piece(draw_list, piece.position.x, piece.position.y, piece.player);
    }
}

void TesterModeTest::ui() {
    vertical_spacing();

    if (ImGui::Button("Compute")) {
        test_state = TestState::ComputerBegin;
    }

    if (ImGui::Button("Stop AI")) {

    }

    if (ImGui::Button("Reset")) {
        reset();
    }

    vertical_spacing();

    ImGui::Text("Piece"); ImGui::SameLine();
    ImGui::RadioButton("White", &piece, PieceWhite); ImGui::SameLine();
    ImGui::RadioButton("Black", &piece, PieceBlack);

    vertical_spacing();

    ImGui::Separator();

    vertical_spacing();

    ImGui::Text("Turn"); ImGui::SameLine();
    static int turn = PieceWhite;

    if (ImGui::RadioButton("White##turn", &turn, PieceWhite)) {
        game_test.turn = Player::White;
    }

    ImGui::SameLine();

    if (ImGui::RadioButton("Black##turn", &turn, PieceBlack)) {
        game_test.turn = Player::Black;
    }

    const float spacing = ImGui::GetStyle().ItemInnerSpacing.x;

    ImGui::PushButtonRepeat(true);
    if (ImGui::ArrowButton("##wleft", ImGuiDir_Left)) { if (game_test.plies > 0) game_test.plies--; }
    ImGui::SameLine(0.0f, spacing);
    if (ImGui::ArrowButton("##wright", ImGuiDir_Right)) { game_test.plies++; }
    ImGui::PopButtonRepeat();
    ImGui::SameLine();
    ImGui::Text("%u plies", game_test.plies);

    vertical_spacing();

    ImGui::Separator();

    vertical_spacing();

    ImGui::Text("Result: %s", result_text.c_str());
    ImGui::Spacing();
    ImGui::Text("%s", statistics_text.c_str());
}

void TesterModeTest::setup() {
    game_test.setup();
}

void TesterModeTest::reset() {
    // TODO stop everything

    game_test = {};
    game_test.setup();
}
