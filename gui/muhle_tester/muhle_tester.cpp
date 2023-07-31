#include <iostream>
#include <math.h>

#include <gui_base.hpp>
#include <just_dl/just_dl.hpp>

#include "muhle_tester.hpp"

void MuhleTester::start() {
    game.setup([this]() {
        this->change_turn();
    });
}

void MuhleTester::update() {
    main_menu_bar();
    main_window();

    load_library_modal();

    // ImGui::ShowDemoWindow();

    game.update_nodes_positions(board_unit, board_offset);

    switch (state) {
        case State::None:
            state = State::HumanTurn;

            break;
        case State::HumanTurn:
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                const ImVec2 position = ImGui::GetMousePos();
                game.user_click(glm::vec2(position.x, position.y));
            }

            break;
        case State::ComputerTurn:
            if (muhle_result.done) {
                switch (muhle_result.result.type) {
                    case muhle::MoveType::Place:
                        game.place_piece(Player::Black, muhle_result.result.place.node_index);
                        break;
                    case muhle::MoveType::Move:
                        game.move_piece(
                            muhle_result.result.move.node_source_index,
                            muhle_result.result.move.node_destination_index
                        );
                        break;
                    case muhle::MoveType::PlaceTake:
                        game.place_piece(Player::Black, muhle_result.result.place_take.node_index);
                        game.take_piece(muhle_result.result.place_take.node_take_index);
                        break;
                    case muhle::MoveType::MoveTake:
                        game.move_piece(
                            muhle_result.result.move_take.node_source_index,
                            muhle_result.result.move_take.node_destination_index
                        );
                        game.take_piece(muhle_result.result.move_take.node_take_index);
                        break;
                }

                muhle->join_thread();

                state = State::ComputerEnd;
            }

            break;
        case State::ComputerBegin: {
            const auto game_position = game.get_position();

            muhle::Position position;
            for (size_t i = 0; i < game_position.size(); i++) {
                position.pieces[i] = static_cast<muhle::Piece>(game_position[i]);
            }
            position.white_pieces_outside = game.white_pieces_outside;
            position.black_pieces_outside = game.black_pieces_outside;

            muhle->search(position, muhle::Player::Black, muhle_result);

            state = State::ComputerTurn;

            break;
        }
        case State::ComputerEnd:
            state = State::HumanTurn;

            break;
    }
}

void MuhleTester::dispose() {
    unload_library();
}

void MuhleTester::draw_piece(ImDrawList* draw_list, float x, float y, Player player) {
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

void MuhleTester::draw_all_pieces(ImDrawList* draw_list) {
    for (const Node& node : game.nodes) {
        if (!node.piece.has_value()) {
            continue;
        }

        const Piece& piece = node.piece.value();

        draw_piece(draw_list, piece.position.x, piece.position.y, piece.player);
    }

    if (game.selected_piece_index != INVALID_INDEX) {
        const Piece& piece = game.nodes[game.selected_piece_index].piece.value();

        draw_list->AddCircle(
            ImVec2(piece.position.x, piece.position.y),
            NODE_RADIUS + 1.0f,
            ImColor(255, 30, 30, 255),
            0,
            2.0f
        );
    }
}

void MuhleTester::reset_game() {
    // TODO stop everything

    game = {};
    game.setup([this]() {
        this->change_turn();
    });

    state = State::None;
}

void MuhleTester::change_turn() {
    switch (state) {
        case State::HumanTurn:
            state = State::ComputerBegin;
            break;
        case State::ComputerEnd:
            state = State::HumanTurn;
            break;
        default:
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

    muhle_intelligence_name = reinterpret_cast<LibraryName>(
        just_dl::load_function(library_handle, "muhle_intelligence_name", err)
    );

    if (err) {
        std::cout << "Could not load functions: " << err.message() << '\n';
        return;
    }

    muhle = muhle_intelligence_create();
    muhle->initialize();

    std::cout << "Successfully loaded library `" << buffer << "`, named `" << muhle_intelligence_name() << "`\n";
}

void MuhleTester::unload_library() {
    if (library_handle == nullptr) {
        return;
    }

    muhle_intelligence_destroy(muhle);

    just_dl::Error err;

    just_dl::close_library(library_handle, err);

    if (err) {
        std::cout << err.message() << '\n';
        return;
    }

    std::cout << "Successfully unloaded library\n";
}

void MuhleTester::main_menu_bar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Load AI")) {
                show_load_library = true;
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

void MuhleTester::load_library_modal() {
    if (show_load_library) {
        ImGui::OpenPopup("Load Library");
        show_load_library = false;
    }

    if (ImGui::BeginPopupModal("Load Library", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char buffer[128] {};
        ImGui::InputText("File Path", buffer, 128);

        if (ImGui::Button("Load")) {
            load_library(buffer);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void MuhleTester::board_canvas() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(100, 100, 100, 255));

    ImGui::BeginChild("Canvas", ImVec2(400.0f, 400.0f));

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 cursor_position = ImGui::GetCursorScreenPos();
    ImVec2 canvas_p0 = ImVec2(cursor_position.x + viewport->WorkPos.x, cursor_position.y + viewport->WorkPos.y);
    ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
    if (canvas_sz.x < 100.0f) canvas_sz.x = 100.0f;
    if (canvas_sz.y < 100.0f) canvas_sz.y = 100.0f;
    ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    board_unit = canvas_p1.x < canvas_p1.y ? canvas_p1.x / 8.0f : canvas_p1.y / 8.0f;
    board_offset = glm::vec2(canvas_p0.x / 2.0f, canvas_p0.y / 2.0f);
    const float UNIT = board_unit;
    const glm::vec2 OFFSET = board_offset;

    const ImColor color = ImColor(220, 220, 220);
    const float thickness = 2.0f;

    draw_list->AddRect(ImVec2(1.0f * UNIT + OFFSET.x, 7.0f * UNIT + OFFSET.y), ImVec2(7.0f * UNIT + OFFSET.x, 1.0f * UNIT + OFFSET.y), color, 0.0f, 0, thickness);
    draw_list->AddRect(ImVec2(2.0f * UNIT + OFFSET.x, 6.0f * UNIT + OFFSET.y), ImVec2(6.0f * UNIT + OFFSET.x, 2.0f * UNIT + OFFSET.y), color, 0.0f, 0, thickness);
    draw_list->AddRect(ImVec2(3.0f * UNIT + OFFSET.x, 5.0f * UNIT + OFFSET.y), ImVec2(5.0f * UNIT + OFFSET.x, 3.0f * UNIT + OFFSET.y), color, 0.0f, 0, thickness);

    draw_list->AddLine(ImVec2(4.0f * UNIT + OFFSET.x, 1.0f * UNIT + OFFSET.y), ImVec2(4.0f * UNIT + OFFSET.x, 3.0f * UNIT + OFFSET.y), color, thickness);
    draw_list->AddLine(ImVec2(5.0f * UNIT + OFFSET.x, 4.0f * UNIT + OFFSET.y), ImVec2(7.0f * UNIT + OFFSET.x, 4.0f * UNIT + OFFSET.y), color, thickness);
    draw_list->AddLine(ImVec2(4.0f * UNIT + OFFSET.x, 5.0f * UNIT + OFFSET.y), ImVec2(4.0f * UNIT + OFFSET.x, 7.0f * UNIT + OFFSET.y), color, thickness);
    draw_list->AddLine(ImVec2(1.0f * UNIT + OFFSET.x, 4.0f * UNIT + OFFSET.y), ImVec2(3.0f * UNIT + OFFSET.x, 4.0f * UNIT + OFFSET.y), color, thickness);

    draw_all_pieces(draw_list);

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

    game_debug();

    ImGui::EndChild();
}

void MuhleTester::bottom_side() {
    game_debug();
}

void MuhleTester::play_mode_buttons() {
    if (ImGui::Button("Stop AI")) {

    }

    if (ImGui::Button("Reset")) {
        reset_game();
    }

    ImGui::Separator();
}

void MuhleTester::test_mode_buttons() {
    if (ImGui::Button("Compute")) {

    }

    if (ImGui::Button("Stop AI")) {

    }

    if (ImGui::Button("Reset")) {
        reset_game();
    }

    ImGui::Separator();
}

void MuhleTester::game_debug() {
    ImGui::BeginChild("Game debug");

    ImGui::Text("White pieces on board: %u", game.white_pieces_on_board);
    ImGui::Text("White pieces outside: %u", game.white_pieces_outside);
    ImGui::Text("Black pieces on board: %u", game.black_pieces_on_board);
    ImGui::Text("Black pieces outside: %u", game.black_pieces_outside);
    ImGui::Text("Turn: %s", game.turn == Player::White ? "white" : "black");
    ImGui::Text("Phase: %d", game.phase);
    ImGui::Text("Ending: %d", game.ending);
    ImGui::Text("Selected piece: %d", game.selected_piece_index);
    ImGui::Text("Can jump: white %d, black %d", game.can_jump[0], game.can_jump[1]);
    ImGui::Text("Must take piece: %d", game.must_take_piece);
    ImGui::Text("Plies without mills: %u", game.plies_without_mills);

    ImGui::EndChild();
}
