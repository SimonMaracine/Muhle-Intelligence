#pragma once

#include <string>
#include <vector>

#include <gui_base/gui_base.hpp>
#include <muhle_intelligence/muhle_intelligence.hpp>
#include <common/board.hpp>

struct MuhlePlayer : public gui_base::GuiApplication {
    MuhlePlayer()
        : gui_base::GuiApplication(1024, 576, reinterpret_cast<const char*>(u8"Mühle Player"), false) {}

    virtual void start() override;
    virtual void update() override;
    virtual void dispose() override;

    void load_font();

    void load_library(const char* buffer);
    void unload_library();

    void main_menu_bar();
    void load_library();
    void about();
    void notation();
    void board();
    void controls();

    MuhleBoard muhle_board;

    ImFont* label_font = nullptr;

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

    bool board_has_focus = true;
};
