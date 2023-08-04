#include <iostream>
#include <string>

#include <gui_base.hpp>
#include <just_dl/just_dl.hpp>
#include <muhle_intelligence/muhle_intelligence.hpp>
#include <muhle_intelligence_miscellaneous/miscellaneous.hpp>

#include "muhle_tester.hpp"

static void vertical_spacing() {
    ImGui::Dummy(ImVec2(0.0f, 4.0f));
}

void MuhleTester::start() {
    game_play.setup([this]() {
        this->change_turn();
    });

    game_test.setup();

    ImGuiIO& io = ImGui::GetIO();

    ImFontConfig config {};
    io.Fonts->AddFontDefault();
    config.SizePixels = 15;
    label_font = io.Fonts->AddFontDefault(&config);
    io.Fonts->Build();
}

void MuhleTester::update() {
    main_menu_bar();
    main_window();

    // ImGui::ShowDemoWindow();

    if (muhle == nullptr) {
        return;
    }

    if (mode == ModePlay) {
        game_play.update_nodes_positions(board_unit, board_offset);
        play_mode_update();
    } else {
        game_test.update_nodes_positions(board_unit, board_offset);
        test_mode_update();
    }
}

void MuhleTester::dispose() {
    unload_library();
}

void MuhleTester::play_mode_update() {
    switch (play_state) {
        case PlayState::NextTurn:
            if (game_play.phase != GamePhase::GameOver) {
                switch (game_play.turn) {
                    case Player::White:
                        switch (white) {
                            case PlayerHuman:
                                play_state = PlayState::HumanThinking;
                                break;
                            case PlayerComputer:
                                play_state = PlayState::ComputerBegin;
                                break;
                        }

                        break;
                    case Player::Black:
                        switch (black) {
                            case PlayerHuman:
                                play_state = PlayState::HumanThinking;
                                break;
                            case PlayerComputer:
                                play_state = PlayState::ComputerBegin;
                                break;
                        }

                        break;
                }
            }

            break;
        case PlayState::HumanThinking:
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                const ImVec2 position = ImGui::GetMousePos();
                game_play.user_click(glm::vec2(position.x, position.y));

                play_state = PlayState::NextTurn;
            }

            break;
        case PlayState::ComputerBegin: {
            if (muhle != nullptr) {
                const auto game_position = game_play.get_position();

                muhle::Position position;
                for (size_t i = 0; i < game_position.size(); i++) {
                    position.pieces[i] = static_cast<muhle::Piece>(game_position[i]);
                }
                position.plies = game_play.plies;

                muhle->search(
                    position,
                    game_play.turn == Player::White ? muhle::Player::White : muhle::Player::Black,
                    muhle_result
                );

                play_state = PlayState::ComputerThinking;
            }

            break;
        }
        case PlayState::ComputerThinking:
            if (muhle_result.done) {
                muhle::print_result_statistics(muhle_result);

                switch (muhle_result.result.type) {
                    case muhle::MoveType::Place:
                        game_play.place_piece(muhle_result.result.place.node_index);
                        break;
                    case muhle::MoveType::Move:
                        game_play.move_piece(
                            muhle_result.result.move.node_source_index,
                            muhle_result.result.move.node_destination_index
                        );
                        break;
                    case muhle::MoveType::PlaceTake:
                        game_play.place_piece(muhle_result.result.place_take.node_index);
                        game_play.take_piece(muhle_result.result.place_take.node_take_index);
                        break;
                    case muhle::MoveType::MoveTake:
                        game_play.move_piece(
                            muhle_result.result.move_take.node_source_index,
                            muhle_result.result.move_take.node_destination_index
                        );
                        game_play.take_piece(muhle_result.result.move_take.node_take_index);
                        break;
                }

                muhle->join_thread();

                computer_move_history.push_back(muhle_result);

                play_state = PlayState::ComputerEnd;
            }

            break;
        case PlayState::ComputerEnd:
            play_state = PlayState::NextTurn;

            break;
    }
}

void MuhleTester::test_mode_update() {
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        const ImVec2 position = ImGui::GetMousePos();
        const Player player = piece == PieceWhite ? Player::White : Player::Black;

        game_test.user_click(glm::vec2(position.x, position.y), GameTest::MouseButton::Left, player);
    }

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
        const ImVec2 position = ImGui::GetMousePos();

        game_test.user_click(glm::vec2(position.x, position.y), GameTest::MouseButton::Right);
    }

    switch (test_state) {
        case TestState::None:
            break;
        case TestState::ComputerBegin: {
            if (muhle != nullptr) {
                result_text = "[None]";

                const auto game_position = game_test.get_position();

                muhle::Position position;
                for (size_t i = 0; i < game_position.size(); i++) {
                    position.pieces[i] = static_cast<muhle::Piece>(game_position[i]);
                }
                position.plies = game_test.plies;

                muhle->search(
                    position,
                    game_test.turn == Player::White ? muhle::Player::White : muhle::Player::Black,
                    muhle_result
                );

                test_state = TestState::ComputerThinking;
            }

            break;
        }
        case TestState::ComputerThinking: {
            if (muhle_result.done) {
                muhle::print_result_statistics(muhle_result);

                result_text = muhle::move_to_string(muhle_result.result);

                muhle->join_thread();

                test_state = TestState::None;
            }

            break;
        }
    }
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
    switch (mode) {
        case ModePlay: {
                for (const Node& node : game_play.nodes) {
                if (!node.piece.has_value()) {
                    continue;
                }

                const Piece& piece = node.piece.value();

                draw_piece(draw_list, piece.position.x, piece.position.y, piece.player);
            }

            if (game_play.selected_piece_index != INVALID_INDEX) {
                const Piece& piece = game_play.nodes[game_play.selected_piece_index].piece.value();

                draw_list->AddCircle(
                    ImVec2(piece.position.x, piece.position.y),
                    NODE_RADIUS + 1.0f,
                    ImColor(255, 30, 30, 255),
                    0,
                    2.0f
                );
            }

            break;
        }
        case ModeTest: {
            for (const Node& node : game_test.nodes) {
                if (!node.piece.has_value()) {
                    continue;
                }

                const Piece& piece = node.piece.value();

                draw_piece(draw_list, piece.position.x, piece.position.y, piece.player);
            }

            break;
        }
    }
}

void MuhleTester::reset_game_play() {
    // TODO stop everything

    game_play = {};
    game_play.setup([this]() {
        this->change_turn();
    });

    play_state = PlayState::NextTurn;

    computer_move_history.clear();
}

void MuhleTester::reset_game_test() {
    // TODO stop everything

    game_test = {};
    game_test.setup();
}

void MuhleTester::change_turn() {
    switch (play_state) {
        case PlayState::HumanThinking:
            play_state = PlayState::ComputerBegin;
            break;
        case PlayState::ComputerEnd:
            play_state = PlayState::HumanThinking;
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
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings
    );

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

    if (ImGui::Begin("Main", nullptr, flags)) {
        board_canvas();
        ImGui::SameLine();
        right_side();

        ImGui::End();
    }

    ImGui::PopStyleVar(2);
}

void MuhleTester::load_library() {
    static char buffer[128] {};
    ImGui::InputText("File Path", buffer, 128);

    if (ImGui::Button("Load")) {
        load_library(buffer);
    }
}

void MuhleTester::about() {
    ImGui::Text(u8"Mühle Tester - for testing and developing Mühle Intelligence");
}

void MuhleTester::notation() {
    ImGui::Text("A player's turn is described as:");
    ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.3f, 1.0f), "player move_type node[-node] [move_type node]");
    ImGui::Text("1. Player: W for white, B for black");
    ImGui::Text("2. Main move: P for place, M for move, first and second phase respectively");
    ImGui::Text("3. Node: one node - place location, or two nodes - move source and destination");
    ImGui::Text("4. Optional, take move: T preceeded by a node - which piece was taken");
}

void MuhleTester::board_canvas() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(90, 90, 90, 255));

    ImGui::BeginChild("Canvas", ImVec2(500.0f, 500.0f));

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

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
}

void MuhleTester::right_side() {
    ImGui::BeginChild("Right side buttons");

    ImGui::Text("AI library name: %s", library_name.c_str());
    ImGui::Separator();

    switch (mode) {
        case ModePlay:
            play_mode_buttons();
            break;
        case ModeTest:
            test_mode_buttons();
            break;
    }

    ImGui::EndChild();
}

void MuhleTester::play_mode_buttons() {
    vertical_spacing();

    if (ImGui::Button("Stop AI")) {

    }

    if (ImGui::Button("Reset")) {
        reset_game_play();
    }

    vertical_spacing();

    ImGui::Text("White"); ImGui::SameLine();
    ImGui::RadioButton("Human##w", &white, PlayerHuman); ImGui::SameLine();
    ImGui::RadioButton("Computer##w", &white, PlayerComputer);

    ImGui::Text("Black"); ImGui::SameLine();
    ImGui::RadioButton("Human##b", &black, PlayerHuman); ImGui::SameLine();
    ImGui::RadioButton("Computer##b", &black, PlayerComputer);

    vertical_spacing();

    ImGui::Separator();

    vertical_spacing();

    ImGui::Text("State: %d", static_cast<int>(play_state));
    ImGui::Text("White pieces on board: %u", game_play.white_pieces_on_board);
    ImGui::Text("White pieces outside: %u", game_play.white_pieces_outside);
    ImGui::Text("Black pieces on board: %u", game_play.black_pieces_on_board);
    ImGui::Text("Black pieces outside: %u", game_play.black_pieces_outside);
    ImGui::Text("Turn: %s", game_play.turn == Player::White ? "white" : "black");
    ImGui::Text("Phase: %d", static_cast<int>(game_play.phase));
    ImGui::Text("Ending: %d", static_cast<int>(game_play.ending));
    ImGui::Text("Plies: %u", game_play.plies);
    ImGui::Text("Selected piece: %d", game_play.selected_piece_index);
    ImGui::Text("Can jump: white %d, black %d", game_play.can_jump[0], game_play.can_jump[1]);
    ImGui::Text("Must take piece: %d", game_play.must_take_piece);
    ImGui::Text("Plies without mills: %u", game_play.plies_without_mills);

    vertical_spacing();

    ImGui::Separator();

    vertical_spacing();

    ImGui::BeginChild("Moves");

    for (const muhle::Result& result : computer_move_history) {
        ImGui::Text("%s", muhle::move_to_string(result.result).c_str());
        ImGui::Spacing();
    }

    ImGui::EndChild();
}

void MuhleTester::test_mode_buttons() {
    vertical_spacing();

    if (ImGui::Button("Compute")) {
        test_state = TestState::ComputerBegin;
    }

    if (ImGui::Button("Stop AI")) {

    }

    if (ImGui::Button("Reset")) {
        reset_game_test();
    }

    vertical_spacing();

    ImGui::Text("Piece"); ImGui::SameLine();
    ImGui::RadioButton("White", &piece, PieceWhite); ImGui::SameLine();
    ImGui::RadioButton("Black", &piece, PieceBlack);

    vertical_spacing();

    ImGui::Separator();

    vertical_spacing();

    ImGui::Text("Turn"); ImGui::SameLine();
    static int turn = PieceWhite;

    if (ImGui::RadioButton("White##turn", &turn, PieceWhite)) {
        game_test.turn = Player::White;
    }

    ImGui::SameLine();

    if (ImGui::RadioButton("Black##turn", &turn, PieceBlack)) {
        game_test.turn = Player::Black;
    }

    const float spacing = ImGui::GetStyle().ItemInnerSpacing.x;

    ImGui::PushButtonRepeat(true);
    if (ImGui::ArrowButton("##wleft", ImGuiDir_Left)) { if (game_test.plies > 0) game_test.plies--; }
    ImGui::SameLine(0.0f, spacing);
    if (ImGui::ArrowButton("##wright", ImGuiDir_Right)) { game_test.plies++; }
    ImGui::PopButtonRepeat();
    ImGui::SameLine();
    ImGui::Text("%u plies", game_test.plies);

    vertical_spacing();

    ImGui::Separator();

    vertical_spacing();

    ImGui::Text("Result: %s", result_text.c_str());
}
