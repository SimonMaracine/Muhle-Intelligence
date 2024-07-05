#include "muhle_player.hpp"

#include <iostream>
#include <string>

#include <gui_base/gui_base.hpp>
#include <ImGuiFileDialog.h>

void MuhlePlayer::start() {
    ImGuiIO& io {ImGui::GetIO()};
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void MuhlePlayer::update() {
    main_menu_bar();
    board();
    controls();
    load_library_dialog();

    switch (state) {
        case State::NextTurn:
            break;
        case State::HumanThinking:
            break;
        case State::ComputerBegin:
            state = State::ComputerThinking;
            break;
        case State::ComputerThinking:
            state = State::NextTurn;
            break;
    }
}

void MuhlePlayer::stop() {
    unload_library();
}

void MuhlePlayer::load_library(const std::string& file_path) {
    if (file_path.empty()) {
        return;
    }
}

void MuhlePlayer::unload_library() {

}

void MuhlePlayer::main_menu_bar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Player")) {
            if (ImGui::MenuItem("Load Engine")) {
                load_library();
            }
            if (ImGui::MenuItem("Reset Board", nullptr, nullptr, state != State::ComputerThinking)) {
                reset();
            }
            if (ImGui::BeginMenu("Import Position")) {
                import_position();

                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Export Position")) {

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
    // ImGuiFileDialog::Instance()->OpenDialog(
    //     "FileDialog",
    //     "Choose File",
    //     ".so,.dll",
    //     ".",
    //     1,
    //     nullptr,
    //     ImGuiFileDialogFlags_Modal
    // );
}

void MuhlePlayer::load_library_dialog() {
    if (ImGuiFileDialog::Instance()->Display("FileDialog", 32, ImVec2(768, 432))) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            const std::string file_path = ImGuiFileDialog::Instance()->GetFilePathName();

            load_library(file_path);
        }

        ImGuiFileDialog::Instance()->Close();
    }
}

void MuhlePlayer::import_position() {
    char buffer[32] {};

    if (ImGui::InputText("string", buffer, 32, ImGuiInputTextFlags_EnterReturnsTrue)) {
        // if (!muhle_board.set_position(buffer)) {
        //     std::cout << "Invalid SMN string\n";
        // }
    }
}

void MuhlePlayer::reset() {
    muhle_board.reset();
    state = State::NextTurn;
}

void MuhlePlayer::about() {
    ImGui::Text("%s", reinterpret_cast<const char*>(u8"Mühle Player - for testing and developing Mühle Intelligence"));
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
    muhle_board.debug();
}

void MuhlePlayer::controls() {
    if (ImGui::Begin("Controls")) {
        ImGui::Text("Engine name: %s", "");
        ImGui::Separator();

        ImGui::Spacing();

        if (ImGui::Button("Stop Engine")) {

        }

        if (state == State::ComputerThinking) {
            ImGui::SameLine();
            ImGui::Text("Thinking...");
        }

        ImGui::Spacing();

        ImGui::Text("White");
        ImGui::SameLine();

        // FIXME don't change state when user made half move

        if (state != State::ComputerThinking) {
            if (ImGui::RadioButton("Human##w", &white, 0)) {
                state = State::NextTurn;
            }

            ImGui::SameLine();

            if (ImGui::RadioButton("Computer##w", &white, 1)) {
                state = State::NextTurn;
            }
        } else {
            ImGui::RadioButton("Human##w", false);
            ImGui::SameLine();
            ImGui::RadioButton("Computer##w", false);
        }

        ImGui::Text("Black");
        ImGui::SameLine();

        if (state != State::ComputerThinking) {
            if (ImGui::RadioButton("Human##b", &black, 0)) {
                state = State::NextTurn;
            }

            ImGui::SameLine();

            if (ImGui::RadioButton("Computer##b", &black, 1)) {
                state = State::NextTurn;
            }
        } else {
            ImGui::RadioButton("Human##b", false);
            ImGui::SameLine();
            ImGui::RadioButton("Computer##b", false);
        }
    }

    ImGui::End();
}
