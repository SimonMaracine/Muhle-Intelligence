#pragma once

#include <string>
#include <vector>

#include <gui_base/gui_base.hpp>
#include <common/board.hpp>

struct MuhlePlayer : public gui_base::GuiApplication {
    explicit MuhlePlayer(const gui_base::WindowProperties& properties)
        : gui_base::GuiApplication(properties) {}

    void start() override;
    void update() override;
    void stop() override;

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

    board::MuhleBoard muhle_board;

    int white {};
    int black {};

    enum class State {
        NextTurn,
        HumanThinking,
        ComputerBegin,
        ComputerThinking
    } state {State::NextTurn};
};
