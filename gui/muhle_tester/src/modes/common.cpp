#include <gui_base/gui_base.hpp>

#include "game.hpp"

void vertical_spacing() {
    ImGui::Dummy(ImVec2(0.0f, 4.0f));
}

void draw_piece(ImDrawList* draw_list, float x, float y, Player player) {
    ImColor color {};

    switch (player) {
        case Player::White:
            color = ImColor(255, 255, 255, 255);
            break;
        case Player::Black:
            color = ImColor(0, 0, 0, 255);
            break;
    }

    draw_list->AddCircleFilled(ImVec2(x, y), NODE_RADIUS, color);
}
