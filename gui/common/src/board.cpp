#include <array>
#include <vector>
#include <filesystem>
#include <algorithm>

#include <gui_base/gui_base.hpp>

#include "common/game.hpp"
#include "common/board.hpp"

void MuhleBoard::update() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    if (ImGui::Begin("Board")) {
        const ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
        ImVec2 canvas_size = ImGui::GetContentRegionAvail();

        canvas_size.x = std::max(canvas_size.x, 400.0f);
        canvas_size.y = std::max(canvas_size.y, 400.0f);

        const ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_size.x, canvas_p0.y + canvas_size.y);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        const float UNIT = canvas_size.x < canvas_size.y ? (canvas_p1.x - canvas_p0.x) / 10.0f : (canvas_p1.y - canvas_p0.y) / 10.0f;
        const ImVec2 OFFSET = ImVec2(canvas_p0.x, canvas_p0.y);

        const ImColor COLOR = ImColor(210, 210, 210);
        const float THICKNESS = 2.0f;

        draw_list->AddRect(ImVec2(2.0f * UNIT + OFFSET.x, 8.0f * UNIT + OFFSET.y), ImVec2(8.0f * UNIT + OFFSET.x, 2.0f * UNIT + OFFSET.y), COLOR, 0.0f, 0, THICKNESS);
        draw_list->AddRect(ImVec2(3.0f * UNIT + OFFSET.x, 7.0f * UNIT + OFFSET.y), ImVec2(7.0f * UNIT + OFFSET.x, 3.0f * UNIT + OFFSET.y), COLOR, 0.0f, 0, THICKNESS);
        draw_list->AddRect(ImVec2(4.0f * UNIT + OFFSET.x, 6.0f * UNIT + OFFSET.y), ImVec2(6.0f * UNIT + OFFSET.x, 4.0f * UNIT + OFFSET.y), COLOR, 0.0f, 0, THICKNESS);

        draw_list->AddLine(ImVec2(5.0f * UNIT + OFFSET.x, 2.0f * UNIT + OFFSET.y), ImVec2(5.0f * UNIT + OFFSET.x, 4.0f * UNIT + OFFSET.y), COLOR, THICKNESS);
        draw_list->AddLine(ImVec2(6.0f * UNIT + OFFSET.x, 5.0f * UNIT + OFFSET.y), ImVec2(8.0f * UNIT + OFFSET.x, 5.0f * UNIT + OFFSET.y), COLOR, THICKNESS);
        draw_list->AddLine(ImVec2(5.0f * UNIT + OFFSET.x, 6.0f * UNIT + OFFSET.y), ImVec2(5.0f * UNIT + OFFSET.x, 8.0f * UNIT + OFFSET.y), COLOR, THICKNESS);
        draw_list->AddLine(ImVec2(2.0f * UNIT + OFFSET.x, 5.0f * UNIT + OFFSET.y), ImVec2(4.0f * UNIT + OFFSET.x, 5.0f * UNIT + OFFSET.y), COLOR, THICKNESS);

        ImGui::PushFont(label_font);

        draw_list->AddText(ImVec2(2.0f * UNIT + OFFSET.x, 1.0f * UNIT + OFFSET.y), COLOR, "A");
        draw_list->AddText(ImVec2(3.0f * UNIT + OFFSET.x, 1.0f * UNIT + OFFSET.y), COLOR, "B");
        draw_list->AddText(ImVec2(4.0f * UNIT + OFFSET.x, 1.0f * UNIT + OFFSET.y), COLOR, "C");
        draw_list->AddText(ImVec2(5.0f * UNIT + OFFSET.x, 1.0f * UNIT + OFFSET.y), COLOR, "D");
        draw_list->AddText(ImVec2(6.0f * UNIT + OFFSET.x, 1.0f * UNIT + OFFSET.y), COLOR, "E");
        draw_list->AddText(ImVec2(7.0f * UNIT + OFFSET.x, 1.0f * UNIT + OFFSET.y), COLOR, "F");
        draw_list->AddText(ImVec2(8.0f * UNIT + OFFSET.x, 1.0f * UNIT + OFFSET.y), COLOR, "G");

        draw_list->AddText(ImVec2(2.0f * UNIT + OFFSET.x, 9.0f * UNIT + OFFSET.y), COLOR, "A");
        draw_list->AddText(ImVec2(3.0f * UNIT + OFFSET.x, 9.0f * UNIT + OFFSET.y), COLOR, "B");
        draw_list->AddText(ImVec2(4.0f * UNIT + OFFSET.x, 9.0f * UNIT + OFFSET.y), COLOR, "C");
        draw_list->AddText(ImVec2(5.0f * UNIT + OFFSET.x, 9.0f * UNIT + OFFSET.y), COLOR, "D");
        draw_list->AddText(ImVec2(6.0f * UNIT + OFFSET.x, 9.0f * UNIT + OFFSET.y), COLOR, "E");
        draw_list->AddText(ImVec2(7.0f * UNIT + OFFSET.x, 9.0f * UNIT + OFFSET.y), COLOR, "F");
        draw_list->AddText(ImVec2(8.0f * UNIT + OFFSET.x, 9.0f * UNIT + OFFSET.y), COLOR, "G");

        draw_list->AddText(ImVec2(9.0f * UNIT + OFFSET.x, 2.0f * UNIT + OFFSET.y), COLOR, "7");
        draw_list->AddText(ImVec2(9.0f * UNIT + OFFSET.x, 3.0f * UNIT + OFFSET.y), COLOR, "6");
        draw_list->AddText(ImVec2(9.0f * UNIT + OFFSET.x, 4.0f * UNIT + OFFSET.y), COLOR, "5");
        draw_list->AddText(ImVec2(9.0f * UNIT + OFFSET.x, 5.0f * UNIT + OFFSET.y), COLOR, "4");
        draw_list->AddText(ImVec2(9.0f * UNIT + OFFSET.x, 6.0f * UNIT + OFFSET.y), COLOR, "3");
        draw_list->AddText(ImVec2(9.0f * UNIT + OFFSET.x, 7.0f * UNIT + OFFSET.y), COLOR, "2");
        draw_list->AddText(ImVec2(9.0f * UNIT + OFFSET.x, 8.0f * UNIT + OFFSET.y), COLOR, "1");

        draw_list->AddText(ImVec2(1.0f * UNIT + OFFSET.x, 2.0f * UNIT + OFFSET.y), COLOR, "7");
        draw_list->AddText(ImVec2(1.0f * UNIT + OFFSET.x, 3.0f * UNIT + OFFSET.y), COLOR, "6");
        draw_list->AddText(ImVec2(1.0f * UNIT + OFFSET.x, 4.0f * UNIT + OFFSET.y), COLOR, "5");
        draw_list->AddText(ImVec2(1.0f * UNIT + OFFSET.x, 5.0f * UNIT + OFFSET.y), COLOR, "4");
        draw_list->AddText(ImVec2(1.0f * UNIT + OFFSET.x, 6.0f * UNIT + OFFSET.y), COLOR, "3");
        draw_list->AddText(ImVec2(1.0f * UNIT + OFFSET.x, 7.0f * UNIT + OFFSET.y), COLOR, "2");
        draw_list->AddText(ImVec2(1.0f * UNIT + OFFSET.x, 8.0f * UNIT + OFFSET.y), COLOR, "1");

        ImGui::PopFont();

        draw_pieces(draw_list);
    }

    ImGui::End();

    ImGui::PopStyleVar();
}

void MuhleBoard::draw_pieces(ImDrawList* draw_list) {

}

void MuhleBoard::load_font() {
    const char* FONT = "LiberationMono-Regular.ttf";

    ImGuiIO& io = ImGui::GetIO();

    if (!std::filesystem::exists(FONT)) {
        ImFontConfig config;

        config.SizePixels = 13.0f;
        io.Fonts->AddFontDefault(&config);

        config.SizePixels = 15.0f;
        label_font = io.Fonts->AddFontDefault(&config);
    } else {
        io.Fonts->AddFontFromFileTTF(FONT, 16.0f);
        label_font = io.Fonts->AddFontFromFileTTF(FONT, 18.0f);
    }

    io.Fonts->Build();
}

std::vector<Move> MuhleBoard::generate_moves() {
    return {};
}
