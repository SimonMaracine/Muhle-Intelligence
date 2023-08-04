#pragma once

#include <string>

#include <glm/glm.hpp>
#include <muhle_intelligence/muhle_intelligence.hpp>

#include "game.hpp"

inline constexpr int PieceWhite = 0;
inline constexpr int PieceBlack = 1;

struct TesterModeTest {
    void update(muhle::MuhleIntelligence* muhle, muhle::Result& muhle_result, float board_unit, glm::vec2 board_offset);
    void draw(ImDrawList* draw_list);
    void ui();
    void setup();
    void reset();

    GameTest game_test;
    int piece = PieceWhite;
    enum class TestState {
        None,
        ComputerBegin,
        ComputerThinking
    } test_state = TestState::None;
    std::string result_text = "[None]";

    muhle::MuhleIntelligence* muhle = nullptr;
    muhle::Result* muhle_result = nullptr;
};
