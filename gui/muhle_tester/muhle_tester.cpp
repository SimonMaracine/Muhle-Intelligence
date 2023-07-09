#include <iostream>
#include <math.h>

#include <gui_base.hpp>

#include "muhle_tester.hpp"

void MuhleTester::start() {

}

void MuhleTester::update() {
    main_menu_bar();
    main_window();

    ImGui::ShowDemoWindow();
}

void MuhleTester::main_menu_bar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Import")) {

            }
            if (ImGui::MenuItem("Export")) {

            }
            if (ImGui::MenuItem("Exit")) {

            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Options")) {
            if (ImGui::BeginMenu("Mode")) {
                ImGui::RadioButton("Play", reinterpret_cast<int*>(&mode), static_cast<int>(Mode::Play));
                ImGui::RadioButton("Test", reinterpret_cast<int*>(&mode), static_cast<int>(Mode::Test));

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {

            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void MuhleTester::main_window() {
    const ImGuiWindowFlags flags = (
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings
    );

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(40.0f, 40.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

    if (ImGui::Begin("Main", nullptr, flags)) {
        board_canvas();
        ImGui::SameLine();
        right_side();

        ImGui::End();
    }

    ImGui::PopStyleVar(2);
}

void MuhleTester::board_canvas() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));      // Disable padding
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(100, 100, 100, 255));  // Set a background color

    ImGui::BeginChild("Canvas", ImVec2(400.0f, 400.0f));

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    // Typically you would use a BeginChild()/EndChild() pair to benefit from a clipping region + own scrolling.
    // Here we demonstrate that this can be replaced by simple offsetting + custom drawing + PushClipRect/PopClipRect() calls.
    // To use a child window instead we could use, e.g:
    //      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));      // Disable padding
    //      ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(50, 50, 50, 255));  // Set a background color
    //      ImGui::BeginChild("canvas", ImVec2(0.0f, 0.0f), true, ImGuiWindowFlags_NoMove);
    //      ImGui::PopStyleColor();
    //      ImGui::PopStyleVar();
    //      [...]
    //      ImGui::EndChild();

    // Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 cursor_position = ImGui::GetCursorScreenPos();
    ImVec2 canvas_p0 = ImVec2(cursor_position.x + viewport->WorkPos.x, cursor_position.y + viewport->WorkPos.y);      // ImDrawList API uses screen coordinates!
    ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
    if (canvas_sz.x < 100.0f) canvas_sz.x = 100.0f;
    if (canvas_sz.y < 100.0f) canvas_sz.y = 100.0f;
    ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // This will catch our interactions
    ImGui::InvisibleButton("Canvas button", canvas_sz, ImGuiButtonFlags_MouseButtonLeft);
    const bool is_hovered = ImGui::IsItemHovered(); // Hovered
    const bool is_active = ImGui::IsItemActive();   // Held
    // const ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y); // Lock scrolled origin
    // const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

    const float UNIT = canvas_p1.x < canvas_p1.y ? canvas_p1.x / 8.0f : canvas_p1.y / 8.0f;
    const ImVec2 OFFSET = ImVec2(canvas_p0.x / 2.0f, canvas_p0.y / 2.0f);

    const ImColor color = ImColor(220, 220, 220);
    const float thickness = 2.0f;

    draw_list->AddRect(ImVec2(1.0f * UNIT + OFFSET.x, 7.0f * UNIT + OFFSET.y), ImVec2(7.0f * UNIT + OFFSET.x, 1.0f * UNIT + OFFSET.y), color, 0.0f, 0, thickness);
    draw_list->AddRect(ImVec2(2.0f * UNIT + OFFSET.x, 6.0f * UNIT + OFFSET.y), ImVec2(6.0f * UNIT + OFFSET.x, 2.0f * UNIT + OFFSET.y), color, 0.0f, 0, thickness);
    draw_list->AddRect(ImVec2(3.0f * UNIT + OFFSET.x, 5.0f * UNIT + OFFSET.y), ImVec2(5.0f * UNIT + OFFSET.x, 3.0f * UNIT + OFFSET.y), color, 0.0f, 0, thickness);

    draw_list->AddLine(ImVec2(4.0f * UNIT + OFFSET.x, 1.0f * UNIT + OFFSET.y), ImVec2(4.0f * UNIT + OFFSET.x, 3.0f * UNIT + OFFSET.y), color, thickness);
    draw_list->AddLine(ImVec2(5.0f * UNIT + OFFSET.x, 4.0f * UNIT + OFFSET.y), ImVec2(7.0f * UNIT + OFFSET.x, 4.0f * UNIT + OFFSET.y), color, thickness);
    draw_list->AddLine(ImVec2(4.0f * UNIT + OFFSET.x, 5.0f * UNIT + OFFSET.y), ImVec2(4.0f * UNIT + OFFSET.x, 7.0f * UNIT + OFFSET.y), color, thickness);
    draw_list->AddLine(ImVec2(1.0f * UNIT + OFFSET.x, 4.0f * UNIT + OFFSET.y), ImVec2(3.0f * UNIT + OFFSET.x, 4.0f * UNIT + OFFSET.y), color, thickness);

    // draw_list->AddPolyline(outer_square, sizeof(outer_square), ImU32(255), 0, 4);
    // draw_list->AddPolyline(middle_square, sizeof(middle_square), ImU32(255), 0, 4);
    // draw_list->AddPolyline(inner_square, sizeof(inner_square), ImU32(255), 0, 4);

    // // Add first and second point
    // if (is_hovered && !adding_line && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    // {
    //     points.push_back(mouse_pos_in_canvas);
    //     points.push_back(mouse_pos_in_canvas);
    //     adding_line = true;
    // }
    // if (adding_line)
    // {
    //     points.back() = mouse_pos_in_canvas;
    //     if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
    //         adding_line = false;
    // }

    // // Pan (we use a zero mouse threshold when there's no context menu)
    // // You may decide to make that threshold dynamic based on whether the mouse is hovering something etc.
    // const float mouse_threshold_for_pan = opt_enable_context_menu ? -1.0f : 0.0f;
    // if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
    // {
    //     scrolling.x += io.MouseDelta.x;
    //     scrolling.y += io.MouseDelta.y;
    // }

    // // Context menu (under default mouse threshold)
    // ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
    // if (opt_enable_context_menu && drag_delta.x == 0.0f && drag_delta.y == 0.0f)
    //     ImGui::OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);
    // if (ImGui::BeginPopup("context"))
    // {
    //     if (adding_line)
    //         points.resize(points.size() - 2);
    //     adding_line = false;
    //     if (ImGui::MenuItem("Remove one", NULL, false, points.Size > 0)) { points.resize(points.size() - 2); }
    //     if (ImGui::MenuItem("Remove all", NULL, false, points.Size > 0)) { points.clear(); }
    //     ImGui::EndPopup();
    // }

    // // Draw grid + all lines in the canvas
    // draw_list->PushClipRect(canvas_p0, canvas_p1, true);
    // if (opt_enable_grid)
    // {
    //     const float GRID_STEP = 64.0f;
    //     for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
    //         draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
    //     for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
    //         draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
    // }
    // for (int n = 0; n < points.Size; n += 2)
    //     draw_list->AddLine(ImVec2(origin.x + points[n].x, origin.y + points[n].y), ImVec2(origin.x + points[n + 1].x, origin.y + points[n + 1].y), IM_COL32(255, 255, 0, 255), 2.0f);
    // draw_list->PopClipRect();

    ImGui::EndChild();
}

void MuhleTester::right_side() {
    ImGui::BeginChild("Buttons");

    switch (mode) {
        case Mode::Play:
            play_mode_buttons();
            break;
        case Mode::Test:
            test_mode_buttons();
            break;
    }

    ImGui::EndChild();
}

void MuhleTester::bottom_side() {

}

void MuhleTester::play_mode_buttons() {
    if (ImGui::Button("Stop AI")) {

    }

    if (ImGui::Button("Reset Game")) {

    }
}

void MuhleTester::test_mode_buttons() {

}
