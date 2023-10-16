#include <array>
#include <vector>

#include <gui_base/gui_base.hpp>

#include "common/game.hpp"
#include "common/board.hpp"

void MuhleBoard::update() {
    if (ImGui::Begin("Board")) {

    }

    ImGui::End();
}

std::vector<Move> MuhleBoard::generate_moves() {
    return {};
}
