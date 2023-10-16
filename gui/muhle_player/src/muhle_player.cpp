#include <iostream>
#include <string>
#include <filesystem>

#include <gui_base/gui_base.hpp>
#include <ImGuiFileDialog.h>
#include <just_dl/just_dl.hpp>
#include <muhle_intelligence/muhle_intelligence.hpp>
#include <muhle_intelligence/miscellaneous.hpp>

#include "muhle_player.hpp"

/*
    TODO
    cache and load last library path location
*/

void MuhlePlayer::start() {

}

void MuhlePlayer::update() {
    main_menu_bar();
    board();
    controls();

    ImGui::ShowDemoWindow();

    if (muhle == nullptr || !board_has_focus) {
        return;
    }
}

void MuhlePlayer::dispose() {
    unload_library();
}

void MuhlePlayer::load_library(const char* buffer) {
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

void MuhlePlayer::unload_library() {
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

void MuhlePlayer::main_menu_bar() {
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

void MuhlePlayer::load_library() {
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

void MuhlePlayer::about() {
    ImGui::Text(
        "%s",
        reinterpret_cast<const char*>(u8"Mühle Player - for testing and developing Mühle Intelligence")
    );
}

void MuhlePlayer::notation() {
    ImGui::Text("A player's turn is described as:");
    ImGui::TextColored(ImVec4(0.8f, 0.3f, 0.4f, 1.0f), "player move_type node[-node] [move_type node]");
    ImGui::Text("1. Player: W for white, B for black");
    ImGui::Text("2. Main move: P for place, M for move, first and second phase respectively");
    ImGui::Text("3. Node: one node - place location, two nodes - move source and destination");
    ImGui::Text("4. Take move: T preceeded by a node - which piece was taken");
}

void MuhlePlayer::board() {
    muhle_board.update();
}

void MuhlePlayer::controls() {
    if (ImGui::Begin("Controls")) {
        ImGui::Text("AI library name: %s", library_name.c_str());
        ImGui::Separator();
    }

    ImGui::End();
}
