#include <array>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <cassert>
#include <cmath>

#include <gui_base/gui_base.hpp>

#include "common/game.hpp"
#include "common/board.hpp"

static const int NODE_POSITIONS[24][2] = {
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

void MuhleBoard::update() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(350.0f, 350.0f));

    if (ImGui::Begin("Board")) {
        const ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
        ImVec2 canvas_size = ImGui::GetContentRegionAvail();

        canvas_size.x = std::max(canvas_size.x, 350.0f);
        canvas_size.y = std::max(canvas_size.y, 350.0f);

        const ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_size.x, canvas_p0.y + canvas_size.y);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        const float UNIT = canvas_size.x < canvas_size.y ? (canvas_p1.x - canvas_p0.x) / 10.0f : (canvas_p1.y - canvas_p0.y) / 10.0f;
        const ImVec2 OFFSET = ImVec2(canvas_p0.x, canvas_p0.y);

        const ImColor COLOR = ImColor(200, 200, 200);
        const float THICKNESS = 2.0f;

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

        update_nodes();
        draw_pieces(draw_list);
        update_input();
    }

    ImGui::End();

    ImGui::PopStyleVar(2);
}

void MuhleBoard::reset() {
    board = {};
    turn = Player::White;
    plies = 0;
    plies_without_mills = 0;
    selected_piece_index = INVALID_INDEX;
    white_pieces_on_board = 0;
    black_pieces_on_board = 0;
    must_take_piece = false;
    legal_moves.clear();
}

void MuhleBoard::update_nodes() {
    for (Idx i = 0; i < 24; i++) {
        board[i].index = i;
        board[i].position = ImVec2(
            NODE_POSITIONS[i][0] * board_unit + board_offset.x,
            NODE_POSITIONS[i][1] * board_unit + board_offset.y
        );
    }
}

void MuhleBoard::update_input() {
    if (!ImGui::IsWindowFocused()) {
        return;
    }

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        const ImVec2 position = ImGui::GetMousePos();
        const Idx index = get_index(position);

        if (index == INVALID_INDEX) {
            return;
        }

        if (white_pieces_on_board < 3 || black_pieces_on_board < 3) {
            // Do nothing, if it's game over
            return;
        }

        if (plies >= 18) {
            if (select_piece(index)) {
                return;
            }
        }

        if (!must_take_piece) {
            // Don't generate new moves, if there is a piece to take
            // Try performing the second half of the previous move instead
            legal_moves = generate_moves();
        }

        for (const Move& move : legal_moves) {
            switch (move.type) {
                case MoveType::Place:
                    try_place(move, index);
                    break;
                case MoveType::Move:
                    try_move(move, selected_piece_index, index);
                    break;
                case MoveType::PlaceTake:
                    try_place_take(move, index, index);  // Parameters are contextual, but it's fine
                    break;
                case MoveType::MoveTake:
                    try_move_take(move, selected_piece_index, index, index);  // Parameters are contextual
                    break;
            }
        }
    }
}

void MuhleBoard::draw_pieces(ImDrawList* draw_list) {
    for (const Node& node : board) {
        switch (node.piece) {
            case Piece::None:
                break;
            case Piece::White:
                draw_list->AddCircleFilled(node.position, board_unit / NODE_RADIUS, ImColor(235, 235, 235, 255));
                break;
            case Piece::Black:
                draw_list->AddCircleFilled(node.position, board_unit / NODE_RADIUS, ImColor(15, 15, 15, 255));
                break;
        }
    }

    if (selected_piece_index != INVALID_INDEX) {
        const Node& node = board[selected_piece_index];

        draw_list->AddCircle(node.position, board_unit / NODE_RADIUS + 1.0f, ImColor(240, 30, 30, 255), 0, 2.0f);
    }
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

Idx MuhleBoard::get_index(ImVec2 position) {
    for (Idx i = 0; i < 24; i++) {
        if (point_in_node(position, board[i], board_unit / NODE_RADIUS)) {
            return board[i].index;
        }
    }

    return INVALID_INDEX;
}

bool MuhleBoard::select_piece(Idx index) {
    if (must_take_piece) {
        // A move is already in process
        return false;
    }

    static const Piece color[2] = { Piece::White, Piece::Black };

    if (selected_piece_index != index) {
        if (board[index].piece == color[static_cast<int>(turn)]) {
            selected_piece_index = index;
            return true;
        }
    } else {
        selected_piece_index = INVALID_INDEX;
        return true;
    }

    return false;
}

void MuhleBoard::try_place(const Move& move, Idx place_index) {
    if (must_take_piece) {
        // A move is already in process
        return;
    }

    if (move.place.place_index != place_index) {
        return;
    }

    if (turn == Player::White) {
        board[place_index].piece = Piece::White;
        white_pieces_on_board++;
    } else {
        board[place_index].piece = Piece::Black;
        black_pieces_on_board++;
    }

    plies++;
    turn = opponent(turn);
}

void MuhleBoard::try_place_take(const Move& move, Idx place_index, Idx take_index) {
    if (must_take_piece) {
        if (move.place_take.take_index != take_index) {
            return;
        }

        board[take_index].piece = Piece::None;

        if (turn == Player::White) {
            black_pieces_on_board--;
        } else {
            white_pieces_on_board--;
        }

        plies++;
        turn = opponent(turn);

        must_take_piece = false;
    } else {
        if (move.place_take.place_index != place_index) {
            return;
        }

        if (turn == Player::White) {
            board[place_index].piece = Piece::White;
            white_pieces_on_board++;
        } else {
            board[place_index].piece = Piece::Black;
            black_pieces_on_board++;
        }

        must_take_piece = true;
    }
}

void MuhleBoard::try_move(const Move& move, Idx source_index, Idx destination_index) {
    if (must_take_piece) {
        // A move is already in process
        return;
    }

    if (move.move.source_index != source_index || move.move.destination_index != destination_index) {
        return;
    }

    std::swap(board[source_index].piece, board[destination_index].piece);

    plies++;
    turn = opponent(turn);

    selected_piece_index = INVALID_INDEX;
}

void MuhleBoard::try_move_take(const Move& move, Idx source_index, Idx destination_index, Idx take_index) {
    if (must_take_piece) {
        if (move.move_take.take_index != take_index) {
            return;
        }

        board[take_index].piece = Piece::None;

        if (turn == Player::White) {
            black_pieces_on_board--;
        } else {
            white_pieces_on_board--;
        }

        plies++;
        turn = opponent(turn);

        must_take_piece = false;
    } else {
        if (move.move_take.source_index != source_index || move.move_take.destination_index != destination_index) {
            return;
        }

        std::swap(board[source_index].piece, board[destination_index].piece);

        selected_piece_index = INVALID_INDEX;

        must_take_piece = true;
    }
}

std::vector<Move> MuhleBoard::generate_moves() {
    std::vector<Move> moves;

    const Piece piece = turn == Player::White ? Piece::White : Piece::Black;

    if (plies < 18) {
        get_moves_phase1(piece, moves);
    } else {
        if (pieces_on_board(piece) == 3) {
            get_moves_phase3(piece, moves);
        } else {
            get_moves_phase2(piece, moves);
        }
    }

    return moves;
}

void MuhleBoard::get_moves_phase1(Piece piece, std::vector<Move>& moves) {
    assert(piece != Piece::None);

    for (Idx i = 0; i < 24; i++) {
        if (board[i].piece != Piece::None) {
            continue;
        }

        make_place_move(piece, i);

        if (is_mill(piece, i)) {
            const Piece opponent = opponent_piece(piece);
            const bool all_in_mills = all_pieces_in_mills(opponent);

            for (Idx j = 0; j < 24; j++) {
                if (board[j].piece != opponent) {
                    continue;
                }

                if (is_mill(opponent, j) && !all_in_mills) {
                    continue;
                }

                moves.push_back(create_place_take(i, j));
            }
        } else {
            moves.push_back(create_place(i));
        }

        unmake_place_move(i);
    }
}

void MuhleBoard::get_moves_phase2(Piece piece, std::vector<Move>& moves) {
    assert(piece != Piece::None);

    for (Idx i = 0; i < 24; i++) {
        if (board[i].piece != piece) {
            continue;
        }

        const auto free_positions = neighbor_free_positions(i);

        for (Idx j = 0; j < static_cast<Idx>(free_positions.size()); j++) {
            make_move_move(piece, i, free_positions[j]);

            if (is_mill(piece, free_positions[j])) {
                const auto opponent = opponent_piece(piece);
                const bool all_in_mills = all_pieces_in_mills(opponent);

                for (Idx k = 0; k < 24; k++) {
                    if (board[k].piece != opponent) {
                        continue;
                    }

                    if (is_mill(opponent, k) && !all_in_mills) {
                        continue;
                    }

                    moves.push_back(create_move_take(i, free_positions[j], k));
                }
            } else {
                moves.push_back(create_move(i, free_positions[j]));
            }

            unmake_move_move(piece, i, free_positions[j]);
        }
    }
}

void MuhleBoard::get_moves_phase3(Piece piece, std::vector<Move>& moves) {
    assert(piece != Piece::None);

    for (Idx i = 0; i < 24; i++) {
        if (board[i].piece != piece) {
            continue;
        }

        for (Idx j = 0; j < 24; j++) {
            if (board[j].piece != Piece::None) {
                continue;
            }

            make_move_move(piece, i, j);

            if (is_mill(piece, j)) {
                const auto opponent = opponent_piece(piece);
                const bool all_in_mills = all_pieces_in_mills(opponent);

                for (Idx k = 0; k < 24; k++) {
                    if (board[k].piece != opponent) {
                        continue;
                    }

                    if (is_mill(opponent, k) && !all_in_mills) {
                        continue;
                    }

                    moves.push_back(create_move_take(i, j, k));
                }
            } else {
                moves.push_back(create_move(i, j));
            }

            unmake_move_move(piece, i, j);
        }
    }
}

unsigned int MuhleBoard::pieces_on_board(Piece piece) {
    assert(piece != Piece::None);

    switch (piece) {
        case Piece::White:
            return white_pieces_on_board;
        case Piece::Black:
            return black_pieces_on_board;
        default:
            break;
    }

    return 0;
}

Player MuhleBoard::opponent(Player player) {
    switch (player) {
        case Player::White:
            return Player::Black;
        case Player::Black:
            return Player::White;
    }

    return {};
}

Piece MuhleBoard::opponent_piece(Piece type) {
    assert(type != Piece::None);

    switch (type) {
        case Piece::White:
            return Piece::Black;
        case Piece::Black:
            return Piece::White;
        default:
            break;
    }

    return {};
}

void MuhleBoard::make_place_move(Piece piece, Idx place_index) {
    board[place_index].piece = piece;
}

void MuhleBoard::unmake_place_move(Idx place_index) {
    board[place_index].piece = Piece::None;
}

void MuhleBoard::make_move_move(Piece piece, Idx source_index, Idx destination_index) {
    board[source_index].piece = Piece::None;
    board[destination_index].piece = piece;
}

void MuhleBoard::unmake_move_move(Piece piece, Idx source_index, Idx destination_index) {
    board[source_index].piece = piece;
    board[destination_index].piece = Piece::None;
}

Move MuhleBoard::create_place(Idx place_index) {
    Move move;
    move.place.place_index = place_index;
    move.type = MoveType::Place;

    return move;
}

Move MuhleBoard::create_move(Idx source_index, Idx destination_index) {
    Move move;
    move.move.source_index = source_index;
    move.move.destination_index = destination_index;
    move.type = MoveType::Move;

    return move;
}

Move MuhleBoard::create_place_take(Idx place_index, Idx take_index) {
    Move move;
    move.place_take.place_index = place_index;
    move.place_take.take_index = take_index;
    move.type = MoveType::PlaceTake;

    return move;
}

Move MuhleBoard::create_move_take(Idx source_index, Idx destination_index, Idx take_index) {
    Move move;
    move.move_take.source_index = source_index;
    move.move_take.destination_index = destination_index;
    move.move_take.take_index = take_index;
    move.type = MoveType::MoveTake;

    return move;
}

#define IS_PC(const_index) (board[const_index].piece == piece)

bool MuhleBoard::is_mill(Piece piece, Idx index) {
    assert(piece != Piece::None);

    switch (index) {
        case 0:
            if (IS_PC(1) && IS_PC(2) || IS_PC(9) && IS_PC(21))
                return true;
            break;
        case 1:
            if (IS_PC(0) && IS_PC(2) || IS_PC(4) && IS_PC(7))
                return true;
            break;
        case 2:
            if (IS_PC(0) && IS_PC(1) || IS_PC(14) && IS_PC(23))
                return true;
            break;
        case 3:
            if (IS_PC(4) && IS_PC(5) || IS_PC(10) && IS_PC(18))
                return true;
            break;
        case 4:
            if (IS_PC(3) && IS_PC(5) || IS_PC(1) && IS_PC(7))
                return true;
            break;
        case 5:
            if (IS_PC(3) && IS_PC(4) || IS_PC(13) && IS_PC(20))
                return true;
            break;
        case 6:
            if (IS_PC(7) && IS_PC(8) || IS_PC(11) && IS_PC(15))
                return true;
            break;
        case 7:
            if (IS_PC(6) && IS_PC(8) || IS_PC(1) && IS_PC(4))
                return true;
            break;
        case 8:
            if (IS_PC(6) && IS_PC(7) || IS_PC(12) && IS_PC(17))
                return true;
            break;
        case 9:
            if (IS_PC(0) && IS_PC(21) || IS_PC(10) && IS_PC(11))
                return true;
            break;
        case 10:
            if (IS_PC(9) && IS_PC(11) || IS_PC(3) && IS_PC(18))
                return true;
            break;
        case 11:
            if (IS_PC(9) && IS_PC(10) || IS_PC(6) && IS_PC(15))
                return true;
            break;
        case 12:
            if (IS_PC(13) && IS_PC(14) || IS_PC(8) && IS_PC(17))
                return true;
            break;
        case 13:
            if (IS_PC(12) && IS_PC(14) || IS_PC(5) && IS_PC(20))
                return true;
            break;
        case 14:
            if (IS_PC(12) && IS_PC(13) || IS_PC(2) && IS_PC(23))
                return true;
            break;
        case 15:
            if (IS_PC(16) && IS_PC(17) || IS_PC(6) && IS_PC(11))
                return true;
            break;
        case 16:
            if (IS_PC(15) && IS_PC(17) || IS_PC(19) && IS_PC(22))
                return true;
            break;
        case 17:
            if (IS_PC(15) && IS_PC(16) || IS_PC(8) && IS_PC(12))
                return true;
            break;
        case 18:
            if (IS_PC(19) && IS_PC(20) || IS_PC(3) && IS_PC(10))
                return true;
            break;
        case 19:
            if (IS_PC(18) && IS_PC(20) || IS_PC(16) && IS_PC(22))
                return true;
            break;
        case 20:
            if (IS_PC(18) && IS_PC(19) || IS_PC(5) && IS_PC(13))
                return true;
            break;
        case 21:
            if (IS_PC(22) && IS_PC(23) || IS_PC(0) && IS_PC(9))
                return true;
            break;
        case 22:
            if (IS_PC(21) && IS_PC(23) || IS_PC(16) && IS_PC(19))
                return true;
            break;
        case 23:
            if (IS_PC(21) && IS_PC(22) || IS_PC(2) && IS_PC(14))
                return true;
            break;
    }

    return false;
}

#define IS_FREE_CHECK(const_index) \
    if (board[const_index].piece == Piece::None) { \
        result.push_back(const_index); \
    }

std::vector<Idx> MuhleBoard::neighbor_free_positions(Idx index) {
    std::vector<Idx> result;
    result.reserve(4);

    switch (index) {
        case 0:
            IS_FREE_CHECK(1)
            IS_FREE_CHECK(9)
            break;
        case 1:
            IS_FREE_CHECK(0)
            IS_FREE_CHECK(2)
            IS_FREE_CHECK(4)
            break;
        case 2:
            IS_FREE_CHECK(1)
            IS_FREE_CHECK(14)
            break;
        case 3:
            IS_FREE_CHECK(4)
            IS_FREE_CHECK(10)
            break;
        case 4:
            IS_FREE_CHECK(1)
            IS_FREE_CHECK(3)
            IS_FREE_CHECK(5)
            IS_FREE_CHECK(7)
            break;
        case 5:
            IS_FREE_CHECK(4)
            IS_FREE_CHECK(13)
            break;
        case 6:
            IS_FREE_CHECK(7)
            IS_FREE_CHECK(11)
            break;
        case 7:
            IS_FREE_CHECK(4)
            IS_FREE_CHECK(6)
            IS_FREE_CHECK(8)
            break;
        case 8:
            IS_FREE_CHECK(7)
            IS_FREE_CHECK(12)
            break;
        case 9:
            IS_FREE_CHECK(0)
            IS_FREE_CHECK(10)
            IS_FREE_CHECK(21)
            break;
        case 10:
            IS_FREE_CHECK(3)
            IS_FREE_CHECK(9)
            IS_FREE_CHECK(11)
            IS_FREE_CHECK(18)
            break;
        case 11:
            IS_FREE_CHECK(6)
            IS_FREE_CHECK(10)
            IS_FREE_CHECK(15)
            break;
        case 12:
            IS_FREE_CHECK(8)
            IS_FREE_CHECK(13)
            IS_FREE_CHECK(17)
            break;
        case 13:
            IS_FREE_CHECK(5)
            IS_FREE_CHECK(12)
            IS_FREE_CHECK(14)
            IS_FREE_CHECK(20)
            break;
        case 14:
            IS_FREE_CHECK(2)
            IS_FREE_CHECK(13)
            IS_FREE_CHECK(23)
            break;
        case 15:
            IS_FREE_CHECK(11)
            IS_FREE_CHECK(16)
            break;
        case 16:
            IS_FREE_CHECK(15)
            IS_FREE_CHECK(17)
            IS_FREE_CHECK(19)
            break;
        case 17:
            IS_FREE_CHECK(12)
            IS_FREE_CHECK(16)
            break;
        case 18:
            IS_FREE_CHECK(10)
            IS_FREE_CHECK(19)
            break;
        case 19:
            IS_FREE_CHECK(16)
            IS_FREE_CHECK(18)
            IS_FREE_CHECK(20)
            IS_FREE_CHECK(22)
            break;
        case 20:
            IS_FREE_CHECK(13)
            IS_FREE_CHECK(19)
            break;
        case 21:
            IS_FREE_CHECK(9)
            IS_FREE_CHECK(22)
            break;
        case 22:
            IS_FREE_CHECK(19)
            IS_FREE_CHECK(21)
            IS_FREE_CHECK(23)
            break;
        case 23:
            IS_FREE_CHECK(14)
            IS_FREE_CHECK(22)
            break;
    }

    return result;
}

bool MuhleBoard::all_pieces_in_mills(Piece piece) {
    for (Idx i = 0; i < 24; i++) {
        if (board[i].piece != piece) {
            continue;
        }

        if (!is_mill(piece, i)) {
            return false;
        }
    }

    return true;
}

bool MuhleBoard::point_in_node(ImVec2 position, const Node& node, float radius) {
    const ImVec2 subtracted = ImVec2(node.position.x - position.x, node.position.y - position.y);
    const float length = std::pow(subtracted.x * subtracted.x + subtracted.y * subtracted.y, 0.5f);

    return length < radius;
}
