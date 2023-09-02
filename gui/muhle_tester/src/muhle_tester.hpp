#pragma once

#include <string>
#include <vector>

#include <gui_base/gui_base.hpp>
#include <glm/glm.hpp>
#include <muhle_intelligence/muhle_intelligence.hpp>

#include "game.hpp"
#include "modes/play.hpp"
#include "modes/test.hpp"

inline constexpr int ModePlay = 0;
inline constexpr int ModeTest = 1;

struct MuhleTester : public gui_base::GuiApplication {
    MuhleTester()
        : gui_base::GuiApplication(1024, 576, reinterpret_cast<const char*>(u8"Mühle Tester"), false) {}

    virtual void start() override;
    virtual void update() override;
    virtual void dispose() override;

    void draw_all_pieces(ImDrawList* draw_list);

    void load_library(const char* buffer);
    void unload_library();

    void main_menu_bar();
    void main_window();
    void load_library();
    void about();
    void notation();

    void board_canvas();
    void right_side();

    int mode = ModePlay;

    float board_unit {};
    glm::vec2 board_offset {};
    ImFont* label_font = nullptr;

    TesterModePlay mode_play;
    TesterModeTest mode_test;

    using LibraryCreate = muhle::MuhleIntelligence*(*)();
    using LibraryDestroy = void(*)(muhle::MuhleIntelligence*);
    using LibraryVersion = const char*(*)();

    LibraryCreate muhle_intelligence_create = nullptr;
    LibraryDestroy muhle_intelligence_destroy = nullptr;
    LibraryVersion muhle_intelligence_version = nullptr;
    void* library_handle = nullptr;

    muhle::MuhleIntelligence* muhle = nullptr;
    muhle::Result muhle_result;
    std::string library_name = "";
};
