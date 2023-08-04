#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <muhle_intelligence/muhle_intelligence.hpp>

#include "game.hpp"

inline constexpr int PlayerHuman = 0;
inline constexpr int PlayerComputer = 1;

struct TesterModePlay {
    void update(muhle::MuhleIntelligence* muhle, muhle::Result& muhle_result, float board_unit, glm::vec2 board_offset);
    void draw(ImDrawList* draw_list);
    void ui();
    void setup();
    void reset();
    void change_turn();

    GamePlay game_play;
    int white = PlayerHuman;
    int black = PlayerComputer;

    enum class PlayState {
        NextTurn,
        HumanThinking,
        ComputerBegin,
        ComputerThinking,
        ComputerEnd
    } play_state = PlayState::NextTurn;

    std::vector<muhle::Result> computer_move_history;
};
