#pragma once

#include <gui_base.hpp>
#include <glm/glm.hpp>

#include "game.hpp"

struct MuhleTester : public gui_base::GuiApplication {
    MuhleTester()
        : gui_base::GuiApplication(1024, 576, "Mühle Tester", false) {}

    virtual void start() override;
    virtual void update() override;

    void draw_piece(ImDrawList* draw_list, float x, float y, Player type);
    void draw_all_pieces(ImDrawList* draw_list);

    void reset_game();
    void change_turn();

    void main_menu_bar();
    void main_window();

    void board_canvas();
    void right_side();
    void bottom_side();

    void play_mode_buttons();
    void test_mode_buttons();

    void game_debug();

    enum class Mode {
        Play,
        Test
    } mode = Mode::Play;

    enum class State {
        None,
        HumanTurn,
        ComputerTurn
    } state = State::None;

    Game game;
    float board_unit = 0.0f;
    glm::vec2 board_offset {};
};
