#include "muhle_player.hpp"

#include <iostream>
#include <cstring>

#include <gui_base/gui_base.hpp>
#include <ImGuiFileDialog.h>

void MuhlePlayer::start() {
    ImGuiIO& io {ImGui::GetIO()};
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    muhle_board = board::MuhleBoard([this](const board::Move& move, board::Player turn) {
        int player {};

        switch (turn) {
            case board::Player::White:
                player = white;
                break;
            case board::Player::Black:
                player = black;
                break;
        }

        switch (player) {
            case PlayerHuman:
                std::cerr << "Writing " << board::move_to_string(move) << '\n';

                if (!muhle_process.write("move " + board::move_to_string(move) + '\n')) {
                    std::cerr << "Could not write command\n";
                }

                state = State::NextTurn;
                break;
            case PlayerComputer:
                break;
        }
    });
}

void MuhlePlayer::update() {
    main_menu_bar();
    board();
    controls();
    load_library_dialog();

    if (muhle_board.get_game_over() != board::GameOver::None) {
        return;
    }

    switch (state) {
        case State::NextTurn: {
            int player {};

            switch (muhle_board.get_turn()) {
                case board::Player::White:
                    player = white;
                    break;
                case board::Player::Black:
                    player = black;
                    break;
            }

            switch (player) {
                case PlayerHuman:
                    state = State::HumanThinking;
                    break;
                case PlayerComputer:
                    state = State::ComputerBegin;
                    break;
            }

            break;
        }
        case State::HumanThinking:
            break;
        case State::ComputerBegin:
            if (!muhle_process.write("go\n")) {
                std::cerr << "Could not write command\n";
            }

            state = State::ComputerThinking;

            break;
        case State::ComputerThinking: {
            std::string data;

            if (muhle_process.read(data)) {
                const auto tokens {parse_message(data.substr(0, data.size() - 1))};

                if (tokens.at(0) == "bestmove") {
                    std::cerr << "Board making engine move " << tokens.at(1) << '\n';

                    const board::Move move {board::move_from_string(tokens.at(1))};

                    switch (move.type) {
                        case board::MoveType::Place:
                            muhle_board.place(move.place.place_index);
                            break;
                        case board::MoveType::PlaceTake:
                            muhle_board.place_take(move.place_take.place_index, move.place_take.take_index);
                            break;
                        case board::MoveType::Move:
                            muhle_board.move(move.move.source_index, move.move.destination_index);
                            break;
                        case board::MoveType::MoveTake:
                            muhle_board.move_take(move.move_take.source_index, move.move_take.destination_index, move.move_take.take_index);
                            break;
                    }

                    state = State::NextTurn;
                }
            }

            break;
        }
    }
}

void MuhlePlayer::stop() {
    unload_library();
}

void MuhlePlayer::load_library(const std::string& file_path) {
    try {
        muhle_process = subprocess::Subprocess(file_path);
    } catch (const subprocess::Error& e) {
        std::cerr << "Could not start subprocess: " << e.what() << '\n';
    }

    if (!muhle_process.write("init\n")) {
        std::cerr << "Could not write command\n";
    }
}

void MuhlePlayer::unload_library() {
    if (!muhle_process.active()) {
        return;
    }

    if (!muhle_process.write("quit\n")) {
        std::cerr << "Could not write command\n";
    }

    muhle_process.wait();
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
    IGFD::FileDialogConfig config;
    config.flags |= ImGuiFileDialogFlags_Modal;

    ImGuiFileDialog::Instance()->OpenDialog(
        "FileDialog",
        "Choose File",
        "((.))",
        config
    );
}

void MuhlePlayer::load_library_dialog() {
    if (ImGuiFileDialog::Instance()->Display("FileDialog", 32, ImVec2(768, 432))) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            const std::string file_path {ImGuiFileDialog::Instance()->GetFilePathName()};

            if (!file_path.empty()) {
                load_library(file_path);
            }
        }

        ImGuiFileDialog::Instance()->Close();
    }
}

void MuhlePlayer::import_position() {
    char buffer[32] {};

    if (ImGui::InputText("string", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
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
    muhle_board.update(state == State::HumanThinking);
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

std::vector<std::string> MuhlePlayer::parse_message(const std::string& message) {
    std::vector<std::string> tokens;

    std::string mutable_buffer {message};

    char* token {std::strtok(mutable_buffer.data(), " \t")};

    while (token != nullptr) {
        tokens.emplace_back(token);

        token = std::strtok(nullptr, " \t");
    }

    return tokens;
}
