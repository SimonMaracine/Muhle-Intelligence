#include <iostream>
#include <string>
#include <filesystem>

#include <gui_base/gui_base.hpp>
#include <ImGuiFileDialog.h>
#include <just_dl/just_dl.hpp>
#include <muhle_intelligence/muhle_intelligence.hpp>
#include <muhle_intelligence/miscellaneous.hpp>

#include "muhle_tester.hpp"

/*
    TODO
    cache and load last library path location
*/

void MuhleTester::start() {
    mode_play.setup();
    mode_test.setup();

    load_font();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
}

void MuhleTester::update() {
    main_menu_bar();
    main_window();

    // ImGui::ShowDemoWindow();

    if (muhle == nullptr || !board_has_focus) {
        return;
    }

    switch (mode) {
        case ModePlay:
            mode_play.update(muhle, muhle_result, board_unit, board_offset);
            break;
        case ModeTest:
            mode_test.update(muhle, muhle_result, board_unit, board_offset);
            break;
    }
}

void MuhleTester::dispose() {
    unload_library();
}

void MuhleTester::load_font() {
    const char* FONT = "LiberationMono-Regular.ttf";

    ImGuiIO& io = ImGui::GetIO();

    if (!std::filesystem::exists(FONT)) {
        std::cout << "Could not find font; using default one\n";

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

void MuhleTester::draw_all_pieces(ImDrawList* draw_list) {
    switch (mode) {
        case ModePlay:
            mode_play.draw(draw_list);
            break;
        case ModeTest:
            mode_test.draw(draw_list);
            break;
    }
}

void MuhleTester::load_library(const char* buffer) {
    if (*buffer == '\0') {
        std::cout << "No input\n";
        return;
    }

    unload_library();

    just_dl::Error err;

    library_handle = just_dl::open_library(buffer, 0, err);

    if (err) {
        std::cout << "Could not open library: " << err.message() << '\n';
        return;
    }

    muhle_intelligence_create = reinterpret_cast<LibraryCreate>(
        just_dl::load_function(library_handle, "muhle_intelligence_create", err)
    );

    muhle_intelligence_destroy = reinterpret_cast<LibraryDestroy>(
        just_dl::load_function(library_handle, "muhle_intelligence_destroy", err)
    );

    muhle_intelligence_version = reinterpret_cast<LibraryVersion>(
        just_dl::load_function(library_handle, "muhle_intelligence_version", err)
    );

    if (err) {
        std::cout << "Could not load functions: " << err.message() << '\n';

        just_dl::close_library(library_handle, err);
        library_handle = nullptr;

        return;
    }

    muhle = muhle_intelligence_create();
    muhle->initialize();

    library_name = muhle_intelligence_version();

    std::cout << "Successfully loaded library `" << buffer << "`, named `" << library_name << "`\n";
}

void MuhleTester::unload_library() {
    if (library_handle == nullptr) {
        return;
    }

    muhle->join_thread();
    muhle_intelligence_destroy(muhle);

    just_dl::Error err;

    just_dl::close_library(library_handle, err);

    if (err) {
        std::cout << err.message() << '\n';
        return;
    }

    std::cout << "Successfully unloaded library named: `" << library_name << "`\n";

    library_name = "[None]";
}

void MuhleTester::main_menu_bar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::BeginMenu("Load AI")) {
                load_library();

                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Import")) {

            }
            if (ImGui::MenuItem("Export")) {

            }
            if (ImGui::MenuItem("Exit")) {
                quit();
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Options")) {
            if (ImGui::BeginMenu("Mode")) {
                ImGui::RadioButton("Play", &mode, ModePlay);
                ImGui::RadioButton("Test", &mode, ModeTest);

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::BeginMenu("About")) {
                about();

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Notation")) {
                notation();

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void MuhleTester::main_window() {
    const ImGuiWindowFlags flags = (
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove
    );

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

    if (ImGui::Begin("Main", nullptr, flags)) {
        ImGui::BeginTable("MainLayout", 2);

        ImGui::TableSetupColumn("MainLayoutColumn1", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("MainLayoutColumn2", ImGuiTableColumnFlags_None);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        board_canvas();

        ImGui::TableNextColumn();

        right_side();

        ImGui::EndTable();

        ImGui::End();
    }

    ImGui::PopStyleVar(2);
}

void MuhleTester::load_library() {
    static char buffer[128] {};
    ImGui::InputText("File Path", buffer, 128);

    ImGui::Spacing();

    if (ImGui::Button("Load")) {
        load_library(buffer);
    }

    ImGui::SameLine();

    if (ImGui::Button("Open File Dialog")) {
        ImGuiFileDialog::Instance()->OpenDialog(
            "FileDialog", "Choose File", ".so,.dll", ".", 1, nullptr, ImGuiFileDialogFlags_Modal
        );

        board_has_focus = false;
    }

    if (ImGuiFileDialog::Instance()->Display("FileDialog", 32, ImVec2(768, 432))) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            const std::string file_path = ImGuiFileDialog::Instance()->GetFilePathName();

            load_library(file_path.c_str());

            board_has_focus = true;
        }

        ImGuiFileDialog::Instance()->Close();
    }
}

void MuhleTester::about() {
    ImGui::Text(
        "%s",
        reinterpret_cast<const char*>(u8"Mühle Tester - for testing and developing Mühle Intelligence")
    );
}

void MuhleTester::notation() {
    ImGui::Text("A player's turn is described as:");
    ImGui::TextColored(ImVec4(0.8f, 0.3f, 0.4f, 1.0f), "player move_type node[-node] [move_type node]");
    ImGui::Text("1. Player: W for white, B for black");
    ImGui::Text("2. Main move: P for place, M for move, first and second phase respectively");
    ImGui::Text("3. Node: one node - place location, two nodes - move source and destination");
    ImGui::Text("4. Take move: T preceeded by a node - which piece was taken");
}

void MuhleTester::board_canvas() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(90, 90, 90, 255));

    ImGui::BeginChild("Canvas", ImVec2(450.0f, 450.0f));

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 cursor_position = ImGui::GetCursorScreenPos();
    ImVec2 canvas_p0 = ImVec2(cursor_position.x + viewport->WorkPos.x, cursor_position.y + viewport->WorkPos.y);
    ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
    if (canvas_sz.x < 100.0f) canvas_sz.x = 100.0f;
    if (canvas_sz.y < 100.0f) canvas_sz.y = 100.0f;
    ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    board_unit = canvas_p1.x < canvas_p1.y ? canvas_p1.x / 10.0f : canvas_p1.y / 10.0f;
    board_offset = glm::vec2(canvas_p0.x / 2.0f, canvas_p0.y / 2.0f);
    const float UNIT = board_unit;
    const glm::vec2 OFFSET = board_offset;

    const ImColor color = ImColor(220, 220, 220);
    const float thickness = 2.0f;

    draw_list->AddRect(ImVec2(2.0f * UNIT + OFFSET.x, 8.0f * UNIT + OFFSET.y), ImVec2(8.0f * UNIT + OFFSET.x, 2.0f * UNIT + OFFSET.y), color, 0.0f, 0, thickness);
    draw_list->AddRect(ImVec2(3.0f * UNIT + OFFSET.x, 7.0f * UNIT + OFFSET.y), ImVec2(7.0f * UNIT + OFFSET.x, 3.0f * UNIT + OFFSET.y), color, 0.0f, 0, thickness);
    draw_list->AddRect(ImVec2(4.0f * UNIT + OFFSET.x, 6.0f * UNIT + OFFSET.y), ImVec2(6.0f * UNIT + OFFSET.x, 4.0f * UNIT + OFFSET.y), color, 0.0f, 0, thickness);

    draw_list->AddLine(ImVec2(5.0f * UNIT + OFFSET.x, 2.0f * UNIT + OFFSET.y), ImVec2(5.0f * UNIT + OFFSET.x, 4.0f * UNIT + OFFSET.y), color, thickness);
    draw_list->AddLine(ImVec2(6.0f * UNIT + OFFSET.x, 5.0f * UNIT + OFFSET.y), ImVec2(8.0f * UNIT + OFFSET.x, 5.0f * UNIT + OFFSET.y), color, thickness);
    draw_list->AddLine(ImVec2(5.0f * UNIT + OFFSET.x, 6.0f * UNIT + OFFSET.y), ImVec2(5.0f * UNIT + OFFSET.x, 8.0f * UNIT + OFFSET.y), color, thickness);
    draw_list->AddLine(ImVec2(2.0f * UNIT + OFFSET.x, 5.0f * UNIT + OFFSET.y), ImVec2(4.0f * UNIT + OFFSET.x, 5.0f * UNIT + OFFSET.y), color, thickness);

    ImGui::PushFont(label_font);

    draw_list->AddText(ImVec2(2.0f * UNIT + OFFSET.x, 1.0f * UNIT + OFFSET.y), color, "A");
    draw_list->AddText(ImVec2(3.0f * UNIT + OFFSET.x, 1.0f * UNIT + OFFSET.y), color, "B");
    draw_list->AddText(ImVec2(4.0f * UNIT + OFFSET.x, 1.0f * UNIT + OFFSET.y), color, "C");
    draw_list->AddText(ImVec2(5.0f * UNIT + OFFSET.x, 1.0f * UNIT + OFFSET.y), color, "D");
    draw_list->AddText(ImVec2(6.0f * UNIT + OFFSET.x, 1.0f * UNIT + OFFSET.y), color, "E");
    draw_list->AddText(ImVec2(7.0f * UNIT + OFFSET.x, 1.0f * UNIT + OFFSET.y), color, "F");
    draw_list->AddText(ImVec2(8.0f * UNIT + OFFSET.x, 1.0f * UNIT + OFFSET.y), color, "G");

    draw_list->AddText(ImVec2(2.0f * UNIT + OFFSET.x, 9.0f * UNIT + OFFSET.y), color, "A");
    draw_list->AddText(ImVec2(3.0f * UNIT + OFFSET.x, 9.0f * UNIT + OFFSET.y), color, "B");
    draw_list->AddText(ImVec2(4.0f * UNIT + OFFSET.x, 9.0f * UNIT + OFFSET.y), color, "C");
    draw_list->AddText(ImVec2(5.0f * UNIT + OFFSET.x, 9.0f * UNIT + OFFSET.y), color, "D");
    draw_list->AddText(ImVec2(6.0f * UNIT + OFFSET.x, 9.0f * UNIT + OFFSET.y), color, "E");
    draw_list->AddText(ImVec2(7.0f * UNIT + OFFSET.x, 9.0f * UNIT + OFFSET.y), color, "F");
    draw_list->AddText(ImVec2(8.0f * UNIT + OFFSET.x, 9.0f * UNIT + OFFSET.y), color, "G");

    draw_list->AddText(ImVec2(9.0f * UNIT + OFFSET.x, 2.0f * UNIT + OFFSET.y), color, "7");
    draw_list->AddText(ImVec2(9.0f * UNIT + OFFSET.x, 3.0f * UNIT + OFFSET.y), color, "6");
    draw_list->AddText(ImVec2(9.0f * UNIT + OFFSET.x, 4.0f * UNIT + OFFSET.y), color, "5");
    draw_list->AddText(ImVec2(9.0f * UNIT + OFFSET.x, 5.0f * UNIT + OFFSET.y), color, "4");
    draw_list->AddText(ImVec2(9.0f * UNIT + OFFSET.x, 6.0f * UNIT + OFFSET.y), color, "3");
    draw_list->AddText(ImVec2(9.0f * UNIT + OFFSET.x, 7.0f * UNIT + OFFSET.y), color, "2");
    draw_list->AddText(ImVec2(9.0f * UNIT + OFFSET.x, 8.0f * UNIT + OFFSET.y), color, "1");

    draw_list->AddText(ImVec2(1.0f * UNIT + OFFSET.x, 2.0f * UNIT + OFFSET.y), color, "7");
    draw_list->AddText(ImVec2(1.0f * UNIT + OFFSET.x, 3.0f * UNIT + OFFSET.y), color, "6");
    draw_list->AddText(ImVec2(1.0f * UNIT + OFFSET.x, 4.0f * UNIT + OFFSET.y), color, "5");
    draw_list->AddText(ImVec2(1.0f * UNIT + OFFSET.x, 5.0f * UNIT + OFFSET.y), color, "4");
    draw_list->AddText(ImVec2(1.0f * UNIT + OFFSET.x, 6.0f * UNIT + OFFSET.y), color, "3");
    draw_list->AddText(ImVec2(1.0f * UNIT + OFFSET.x, 7.0f * UNIT + OFFSET.y), color, "2");
    draw_list->AddText(ImVec2(1.0f * UNIT + OFFSET.x, 8.0f * UNIT + OFFSET.y), color, "1");

    ImGui::PopFont();

    draw_all_pieces(draw_list);

    ImGui::EndChild();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}

void MuhleTester::right_side() {
    ImGui::Text("AI library name: %s", library_name.c_str());
    ImGui::Separator();

    switch (mode) {
        case ModePlay:
            mode_play.ui();
            break;
        case ModeTest:
            mode_test.ui();
            break;
    }
}
