#pragma once

#include <gui_base.hpp>
#include <glm/glm.hpp>
#include <muhle_intelligence/muhle_intelligence.hpp>

#include "game.hpp"

inline constexpr int ModePlay = 0;
inline constexpr int ModeTest = 1;

inline constexpr int PlayerHuman = 0;
inline constexpr int PlayerComputer = 1;

inline constexpr int PieceWhite = 0;
inline constexpr int PieceBlack = 1;

struct MuhleTester : public gui_base::GuiApplication {
    MuhleTester()
        : gui_base::GuiApplication(1024, 576, u8"Mühle Tester", false) {}

    virtual void start() override;
    virtual void update() override;
    virtual void dispose() override;

    void play_mode_update();
    void test_mode_update();

    void draw_piece(ImDrawList* draw_list, float x, float y, Player type);
    void draw_all_pieces(ImDrawList* draw_list);

    void reset_game_play();
    void reset_game_test();
    void change_turn();
    void load_library(const char* buffer);
    void unload_library();

    void main_menu_bar();
    void main_window();
    void load_library();
    void about();
    void notation();

    void board_canvas();
    void right_side();

    void play_mode_buttons();
    void test_mode_buttons();

    int mode = ModePlay;

    float board_unit {};
    glm::vec2 board_offset {};
    ImFont* label_font = nullptr;

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

    GameTest game_test;
    int piece = PieceWhite;
    enum class TestState {
        None,
        ComputerBegin,
        ComputerThinking
    } test_state = TestState::None;
    std::string result_text = "[None]";

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
