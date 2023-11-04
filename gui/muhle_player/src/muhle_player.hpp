#pragma once

#include <string>
#include <vector>

#include <gui_base/gui_base.hpp>
#include <muhle_intelligence/muhle_intelligence.hpp>
#include <common/board.hpp>

inline constexpr int PlayerHuman = 0;
inline constexpr int PlayerComputer = 1;

struct MuhlePlayer : public gui_base::GuiApplication {
    MuhlePlayer()
        : gui_base::GuiApplication(1024, 576, reinterpret_cast<const char*>(u8"Mühle Player")) {}

    virtual void start() override;
    virtual void update() override;
    virtual void dispose() override;

    void load_library(const std::string& file_path);
    void unload_library();

    void main_menu_bar();
    void load_library();
    void load_library_dialog();
    void import_position();
    void reset();
    void about();
    void notation();
    void board();
    void controls();
    muhle::Move move_to_muhle_move(const Move& move);

    MuhleBoard muhle_board;

    int white = PlayerHuman;
    int black = PlayerComputer;

    enum class State {
        NextTurn,
        HumanThinking,
        ComputerBegin,
        ComputerThinking,
    } state = State::NextTurn;

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
