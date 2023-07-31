#pragma once

#include <gui_base.hpp>
#include <glm/glm.hpp>
#include <muhle_intelligence/muhle_intelligence.hpp>

#include "game.hpp"

inline constexpr int Play = 0;
inline constexpr int Test = 1;

inline constexpr int Human = 0;
inline constexpr int Computer = 1;

struct MuhleTester : public gui_base::GuiApplication {
    MuhleTester()
        : gui_base::GuiApplication(1024, 576, "Mühle Tester", false) {}

    virtual void start() override;
    virtual void update() override;
    virtual void dispose() override;

    void play_mode_update();
    void test_mode_update();

    void draw_piece(ImDrawList* draw_list, float x, float y, Player type);
    void draw_all_pieces(ImDrawList* draw_list);

    void reset_game();
    void change_turn();
    void load_library(const char* buffer);
    void unload_library();

    void main_menu_bar();
    void main_window();
    void load_library_modal();

    void board_canvas();
    void right_side();

    void play_mode_buttons();
    void test_mode_buttons();

    void game_debug();

    bool show_load_library = false;

    int mode = Play;

    enum class State {
        NextTurn,
        HumanThinking,
        ComputerBegin,
        ComputerThinking,
        ComputerEnd
    } state = State::NextTurn;

    int white = Human;
    int black = Computer;

    Game game;
    float board_unit = 0.0f;
    glm::vec2 board_offset {};

    using LibraryCreate = muhle::MuhleIntelligence*(*)();
    using LibraryDestroy = void(*)(muhle::MuhleIntelligence*);
    using LibraryName = const char*(*)();

    LibraryCreate muhle_intelligence_create = nullptr;
    LibraryDestroy muhle_intelligence_destroy = nullptr;
    LibraryName muhle_intelligence_name = nullptr;
    void* library_handle = nullptr;

    muhle::MuhleIntelligence* muhle = nullptr;
    const char* library_name = "";

    muhle::Result muhle_result;
};
