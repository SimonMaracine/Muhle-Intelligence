#include "common/board.hpp"

#include <cmath>

namespace board {
    static const int NODE_POSITIONS[24][2] {
        { 2, 2 },
        { 5, 2 },
        { 8, 2 },
        { 3, 3 },
        { 5, 3 },
        { 7, 3 },
        { 4, 4 },
        { 5, 4 },
        { 6, 4 },
        { 2, 5 },
        { 3, 5 },
        { 4, 5 },
        { 6, 5 },
        { 7, 5 },
        { 8, 5 },
        { 4, 6 },
        { 5, 6 },
        { 6, 6 },
        { 3, 7 },
        { 5, 7 },
        { 7, 7 },
        { 2, 8 },
        { 5, 8 },
        { 8, 8 }
    };

    void MuhleBoard::update(bool user_input) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(350.0f, 350.0f));

        if (ImGui::Begin("Board")) {
            const ImVec2 canvas_p0 {ImGui::GetCursorScreenPos()};
            ImVec2 canvas_size {ImGui::GetContentRegionAvail()};

            canvas_size.x = std::max(canvas_size.x, 350.0f);
            canvas_size.y = std::max(canvas_size.y, 350.0f);

            const ImVec2 canvas_p1 {ImVec2(canvas_p0.x + canvas_size.x, canvas_p0.y + canvas_size.y)};

            ImDrawList* draw_list {ImGui::GetWindowDrawList()};

            const float UNIT {canvas_size.x < canvas_size.y ? (canvas_p1.x - canvas_p0.x) / 10.0f : (canvas_p1.y - canvas_p0.y) / 10.0f};
            const ImVec2 OFFSET {ImVec2(canvas_p0.x, canvas_p0.y)};

            const ImColor COLOR {ImColor(200, 200, 200)};
            const float THICKNESS {2.0f};

            board_unit = UNIT;
            board_offset = OFFSET;

            draw_list->AddRectFilled(canvas_p0, canvas_p1, ImColor(45, 45, 45));

            draw_list->AddRect(ImVec2(2.0f * UNIT + OFFSET.x, 8.0f * UNIT + OFFSET.y), ImVec2(8.0f * UNIT + OFFSET.x, 2.0f * UNIT + OFFSET.y), COLOR, 0.0f, 0, THICKNESS);
            draw_list->AddRect(ImVec2(3.0f * UNIT + OFFSET.x, 7.0f * UNIT + OFFSET.y), ImVec2(7.0f * UNIT + OFFSET.x, 3.0f * UNIT + OFFSET.y), COLOR, 0.0f, 0, THICKNESS);
            draw_list->AddRect(ImVec2(4.0f * UNIT + OFFSET.x, 6.0f * UNIT + OFFSET.y), ImVec2(6.0f * UNIT + OFFSET.x, 4.0f * UNIT + OFFSET.y), COLOR, 0.0f, 0, THICKNESS);

            draw_list->AddLine(ImVec2(5.0f * UNIT + OFFSET.x, 2.0f * UNIT + OFFSET.y), ImVec2(5.0f * UNIT + OFFSET.x, 4.0f * UNIT + OFFSET.y), COLOR, THICKNESS);
            draw_list->AddLine(ImVec2(6.0f * UNIT + OFFSET.x, 5.0f * UNIT + OFFSET.y), ImVec2(8.0f * UNIT + OFFSET.x, 5.0f * UNIT + OFFSET.y), COLOR, THICKNESS);
            draw_list->AddLine(ImVec2(5.0f * UNIT + OFFSET.x, 6.0f * UNIT + OFFSET.y), ImVec2(5.0f * UNIT + OFFSET.x, 8.0f * UNIT + OFFSET.y), COLOR, THICKNESS);
            draw_list->AddLine(ImVec2(2.0f * UNIT + OFFSET.x, 5.0f * UNIT + OFFSET.y), ImVec2(4.0f * UNIT + OFFSET.x, 5.0f * UNIT + OFFSET.y), COLOR, THICKNESS);

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

            // update_nodes();
            // draw_pieces(draw_list);

            if (user_input) {
                update_user_input();
            }
        }

        ImGui::End();

        ImGui::PopStyleVar(2);
    }

    void MuhleBoard::reset(std::string_view position_string) {

    }

    void MuhleBoard::place(Idx place_index) {

    }

    void MuhleBoard::place_take(Idx place_index, Idx take_index) {

    }

    void MuhleBoard::move(Idx source_index, Idx destination_index) {

    }

    void MuhleBoard::move_take(Idx source_index, Idx destination_index, Idx take_index) {

    }

    void MuhleBoard::update_user_input() {
        if (!ImGui::IsWindowFocused()) {
            return;
        }

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            const ImVec2 position {ImGui::GetMousePos()};
            const Idx index {get_index(position)};


        }
    }

    Idx MuhleBoard::get_index(ImVec2 position) {
        for (Idx i {0}; i < 24; i++) {
            if (point_in_circle(position, board[i], board_unit / NODE_RADIUS)) {
                return i;
            }
        }

        return NULL_INDEX;
    }

    bool MuhleBoard::point_in_circle(ImVec2 point, ImVec2 circle, float radius) {
        const ImVec2 subtracted {ImVec2(circle.x - point.x, circle.y - point.y)};
        const float length {std::pow(subtracted.x * subtracted.x + subtracted.y * subtracted.y, 0.5f)};

        return length < radius;
    }
}
