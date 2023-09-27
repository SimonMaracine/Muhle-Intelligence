#include <optional>
#include <array>
#include <forward_list>
#include <functional>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <cstddef>

#include <glm/glm.hpp>
#include <muhle_intelligence/definitions.hpp>

#include "game.hpp"

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

static const int MILLS_COUNT = 16;
static const int MILLS[MILLS_COUNT][3] = {
    { 0, 1, 2 }, { 2, 14, 23 }, { 21, 22, 23 }, { 0, 9, 21 },
    { 3, 4, 5 }, { 5, 13, 20 }, { 18, 19, 20 }, { 3, 10, 18 },
    { 6, 7, 8 }, { 8, 12, 17 }, { 15, 16, 17 }, { 6, 11, 15 },
    { 1, 4, 7 }, { 12, 13, 14 }, { 16, 19, 22 }, { 9, 10, 11 }
};

static bool point_in_node(glm::vec2 position, const Node& node) {
    const glm::vec2 sub = node.position - position;

    return glm::length(sub) < NODE_RADIUS;
}

static muhle::Position position(const std::array<Node, 24>& nodes, unsigned int plies, Player player) {
    muhle::Position result;

    for (std::size_t i = 0; i < nodes.size(); i++) {
        if (nodes[i].piece.has_value()) {
            result.board[i] = (
                nodes[i].piece->player == Player::White ? muhle::Piece::White : muhle::Piece::Black
            );
        } else {
            result.board[i] = muhle::Piece::None;
        }
    }

    result.player = player == Player::White ? muhle::Player::White : muhle::Player::Black;
    result.plies = plies;

    return result;
}

void GamePlay::setup(MoveLogging::ChangeTurnCallback callback) {
    for (std::size_t i = 0; i < nodes.size(); i++) {
        nodes[i].index = static_cast<int>(i);
    }

    log.callback = callback;
}

void GamePlay::update_nodes_positions(float board_unit, glm::vec2 board_offset) {
    for (std::size_t i = 0; i < nodes.size(); i++) {
        nodes[i].position = glm::vec2(
            NODE_POSITIONS[i][0] * board_unit + board_offset.x,
            NODE_POSITIONS[i][1] * board_unit + board_offset.y
        );

        if (nodes[i].piece.has_value()) {
            nodes[i].piece->position = nodes[i].position;
        }
    }
}

void GamePlay::user_action(glm::vec2 position) {
    switch (phase) {
        case GamePhase::PlacePieces:
            if (must_take_piece) {
                check_take_piece(position);
            } else {
                check_place_piece(position);
            }

            break;
        case GamePhase::MovePieces:
            if (must_take_piece) {
                check_take_piece(position);
            } else {
                check_select_piece(position);
                check_move_piece(position);
            }

            break;
        case GamePhase::GameOver:
            // Do nothing

            break;
    }
}

muhle::Position GamePlay::get_position() {
    return position(nodes, plies, turn);
}

const char* GamePlay::player_to_string() {
    return turn == Player::White ? "white" : "black";
}

const char* GamePlay::phase_to_string() {
    switch (phase) {
        case GamePhase::PlacePieces:
            return "PlacePieces";
        case GamePhase::MovePieces:
            return "MovePieces";
        case GamePhase::GameOver:
            return "GameOver";
    }

    return nullptr;
}

const char* GamePlay::ending_to_string() {
    switch (ending) {
        case Ending::None:
            return "None";
        case Ending::WinnerWhite:
            return "WinnerWhite";
        case Ending::WinnerBlack:
            return "WinnerBlack";
        case Ending::TieBetweenBothPlayers:
            return "TieBetweenBothPlayers";
    }

    return nullptr;
}

void GamePlay::place_piece(int node_index) {
    Node& node = nodes[node_index];

    assert(node.piece == std::nullopt);

    if (turn == Player::White) {
        Piece piece;
        piece.player = Player::White;
        piece.position = node.position;
        node.piece = std::make_optional(piece);

        white_pieces_on_board++;
        white_pieces_outside--;
    } else {
        Piece piece;
        piece.player = Player::Black;
        piece.position = node.position;
        node.piece = std::make_optional(piece);

        black_pieces_on_board++;
        black_pieces_outside--;
    }

    std::cout << "Placed piece\n";
    record_place_move(node_index);

    // Check for mills
    if (piece_in_mill(node, turn)) {
        must_take_piece = true;
        std::cout << "Must take piece\n";
        return;
    }

    // Check game over; only by blocking
    if (player_has_no_legal_moves(turn)) {
        game_over(turn == Player::White ? Ending::WinnerBlack : Ending::WinnerWhite);
        return;
    }

    // Only now check if the game is too long
    if (change_turn() == MAX_PLIES_WITHOUT_MILLS) {
        game_over(Ending::TieBetweenBothPlayers);
        return;
    }

    if (plies == 18) {
        phase_two();

        // Now threefold repetition rule can apply
        threefold_repetition();
    }
}

void GamePlay::move_piece(int node_source_index, int node_destination_index) {
    Node& node_src = nodes[node_source_index];
    Node& node_dest = nodes[node_destination_index];

    assert(node_src.piece != std::nullopt);
    assert(node_dest.piece == std::nullopt);

    std::swap(node_src.piece, node_dest.piece);

    std::cout << "Moved piece\n";
    record_move_move(node_source_index, node_destination_index);

    // Check for mills
    if (piece_in_mill(node_dest, turn)) {
        must_take_piece = true;
        std::cout << "Must take piece\n";
        return;
    }

    // Check game over by blocking
    if (player_has_no_legal_moves(opponent(turn))) {
        game_over(turn == Player::White ? Ending::WinnerWhite : Ending::WinnerBlack);
        return;
    }

    // Check if the game is too long
    if (change_turn() == MAX_PLIES_WITHOUT_MILLS) {
        game_over(Ending::TieBetweenBothPlayers);
        return;
    }

    // Only now check game over by repetition
    if (threefold_repetition()) {
        game_over(Ending::TieBetweenBothPlayers);
        return;
    }
}

void GamePlay::take_piece(int node_index) {
    Node& node = nodes[node_index];

    assert(node.piece != std::nullopt);

    node.piece = std::nullopt;

    std::cout << "Took piece\n";
    record_take_move(node_index);

    if (turn == Player::White) {
        black_pieces_on_board--;
    } else  {
        white_pieces_on_board--;
    }

    must_take_piece = false;

    // Don't check for tie this time
    change_turn();

    // Must reset counter here
    plies_without_mills = 0;

    // Check jumping
    if (player_has_three_pieces(turn)) {
        can_jump[static_cast<std::size_t>(turn)] = true;
    }

    // Check game over
    if (player_has_two_pieces(turn) || player_has_no_legal_moves(turn)) {
        game_over(turn == Player::White ? Ending::WinnerBlack : Ending::WinnerWhite);
        return;
    }

    // Previous positions can occur no more
    clear_repetition();

    // Check for phase two here too
    if (plies == 18) {
        phase_two();

        // Now threefold repetition rule can apply
        threefold_repetition();
    }
}

void GamePlay::check_select_piece(glm::vec2 position) {
    for (std::size_t i = 0; i < nodes.size(); i++) {
        const Node& node = nodes[i];

        if (!point_in_node(position, node)) {
            continue;
        }

        // This is the hovered node

        if (static_cast<int>(i) == selected_piece_index) {
            // Deselect
            selected_piece_index = INVALID_INDEX;
            break;
        }

        if (node.piece == std::nullopt) {
            break;
        }

        if (node.piece->player != turn) {
            break;
        }

        // Selecting a piece

        selected_piece_index = static_cast<int>(i);

        std::cout << "Selected piece index " << i << '\n';

        break;
    }
}

void GamePlay::check_place_piece(glm::vec2 position) {
    for (Node& node : nodes) {
        if (!point_in_node(position, node)) {
            continue;
        }

        // This is the hovered node

        if (node.piece != std::nullopt) {
            break;
        }

        // Placing a piece

        place_piece(node.index);

        break;
    }
}

void GamePlay::check_move_piece(glm::vec2 position) {
    if (selected_piece_index == INVALID_INDEX) {
        return;
    }

    Node& node_src = nodes[selected_piece_index];

    for (Node& node_dest : nodes) {
        if (!point_in_node(position, node_dest)) {
            continue;
        }

        // This is the hovered node

        if (node_dest.piece != std::nullopt) {
            break;
        }

        if (!can_potentially_move(node_src, node_dest)) {
            break;
        }

        // Moving a piece

        move_piece(node_src.index, node_dest.index);

        // Just reset this
        selected_piece_index = INVALID_INDEX;

        break;
    }
}

void GamePlay::check_take_piece(glm::vec2 position) {
    assert(must_take_piece);

    for (Node& node : nodes) {
        if (!point_in_node(position, node)) {
            continue;
        }

        // This is the hovered node

        if (node.piece == std::nullopt) {
            break;
        }

        // It must be opponent piece
        if (node.piece->player == turn) {
            break;
        }

        // If a piece is in mill and not all pieces on the board are in mills
        const Player player = opponent(turn);
        unsigned int player_pieces = (
            player == Player::White ? white_pieces_on_board : black_pieces_on_board
        );

        if (piece_in_mill(node, player) && pieces_in_mills(player) != player_pieces) {
            break;
        }

        // Taking a piece

        take_piece(node.index);

        break;
    }
}

unsigned int GamePlay::change_turn() {
    // Log move
    log.callback(log.current_move.move, log.current_move.player);
    log.current_move = {};

    turn = opponent(turn);

    plies++;
    plies_without_mills++;

    return plies_without_mills;
}

Player GamePlay::opponent(Player player) {
    if (player == Player::White) {
        return Player::Black;
    } else {
        return Player::White;
    }
}

bool GamePlay::can_potentially_move(Node& node_src, Node& node_dest) {
    assert(node_src.piece != std::nullopt);
    assert(node_dest.piece == std::nullopt);

    if (can_jump[static_cast<std::size_t>(turn)]) {
        return true;
    }

    switch (node_src.index) {
        case 0:
            if (node_dest.index == 1 || node_dest.index == 9)
                return true;
            break;
        case 1:
            if (node_dest.index == 0 || node_dest.index == 2 || node_dest.index == 4)
                return true;
            break;
        case 2:
            if (node_dest.index == 1 || node_dest.index == 14)
                return true;
            break;
        case 3:
            if (node_dest.index == 4 || node_dest.index == 10)
                return true;
            break;
        case 4:
            if (node_dest.index == 1 || node_dest.index == 3 || node_dest.index == 5
                    || node_dest.index == 7)
                return true;
            break;
        case 5:
            if (node_dest.index == 4 || node_dest.index == 13)
                return true;
            break;
        case 6:
            if (node_dest.index == 7 || node_dest.index == 11)
                return true;
            break;
        case 7:
            if (node_dest.index == 4 || node_dest.index == 6 || node_dest.index == 8)
                return true;
            break;
        case 8:
            if (node_dest.index == 7 || node_dest.index == 12)
                return true;
            break;
        case 9:
            if (node_dest.index == 0 || node_dest.index == 10 || node_dest.index == 21)
                return true;
            break;
        case 10:
            if (node_dest.index == 3 || node_dest.index == 9 || node_dest.index == 11
                    || node_dest.index == 18)
                return true;
            break;
        case 11:
            if (node_dest.index == 6 || node_dest.index == 10 || node_dest.index == 15)
                return true;
            break;
        case 12:
            if (node_dest.index == 8 || node_dest.index == 13 || node_dest.index == 17)
                return true;
            break;
        case 13:
            if (node_dest.index == 5 || node_dest.index == 12 || node_dest.index == 14
                    || node_dest.index == 20)
                return true;
            break;
        case 14:
            if (node_dest.index == 2 || node_dest.index == 13 || node_dest.index == 23)
                return true;
            break;
        case 15:
            if (node_dest.index == 11 || node_dest.index == 16)
                return true;
            break;
        case 16:
            if (node_dest.index == 15 || node_dest.index == 17 || node_dest.index == 19)
                return true;
            break;
        case 17:
            if (node_dest.index == 12 || node_dest.index == 16)
                return true;
            break;
        case 18:
            if (node_dest.index == 10 || node_dest.index == 19)
                return true;
            break;
        case 19:
            if (node_dest.index == 16 || node_dest.index == 18 || node_dest.index == 20
                    || node_dest.index == 22)
                return true;
            break;
        case 20:
            if (node_dest.index == 13 || node_dest.index == 19)
                return true;
            break;
        case 21:
            if (node_dest.index == 9 || node_dest.index == 22)
                return true;
            break;
        case 22:
            if (node_dest.index == 19 || node_dest.index == 21 || node_dest.index == 23)
                return true;
            break;
        case 23:
            if (node_dest.index == 14 || node_dest.index == 22)
                return true;
            break;
    }

    return false;
}

bool GamePlay::piece_in_mill(const Node& node, Player player) {
    assert(node.piece != std::nullopt);
    assert(node.piece->player == player);

    for (int i = 0; i < MILLS_COUNT; i++) {
        const int* mill = MILLS[i];

        if (node.index != mill[0] && node.index != mill[1] && node.index != mill[2]) {
            continue;
        }

        const auto& piece0 = nodes[mill[0]].piece;
        const auto& piece1 = nodes[mill[1]].piece;
        const auto& piece2 = nodes[mill[2]].piece;

        if (!piece0.has_value() || !piece1.has_value() || !piece2.has_value()) {
            continue;
        }

        if (piece0->player != player || piece1->player != player || piece2->player != player) {
            continue;
        }

        if (piece0->player == piece1->player && piece1->player == piece2->player) {
            return true;
        }
    }

    return false;
}

unsigned int GamePlay::pieces_in_mills(Player player) {
    unsigned int result = 0;

    for (const Node& node : nodes) {
        if (!node.piece.has_value()) {
            continue;
        }

        if (node.piece->player != player) {
            continue;
        }

        const bool in_mill = piece_in_mill(node, player);
        result += static_cast<unsigned int>(in_mill);
    }

    return result;
}

bool GamePlay::player_has_two_pieces(Player player) {
    if (!is_phase_two()) {
        return false;
    }

    if (player == Player::White) {
        return white_pieces_on_board == 2;
    } else {
        return black_pieces_on_board == 2;
    }
}

bool GamePlay::player_has_three_pieces(Player player) {
    if (!is_phase_two()) {
        return false;
    }

    if (player == Player::White) {
        return white_pieces_on_board == 3;
    } else {
        return black_pieces_on_board == 3;
    }
}

bool GamePlay::player_has_no_legal_moves(Player player) {
    if (can_jump[static_cast<std::size_t>(player)]) {
        return false;
    }

    if (!is_phase_two()) {
        return false;
    }

    for (const Node& node : nodes) {
        if (node.piece == std::nullopt) {
            continue;
        }

        if (node.piece->player != player) {
            continue;
        }

        switch (node.index) {
            case 0: {
                const Node& node1 = nodes[1];
                const Node& node2 = nodes[9];
                if (node1.piece == std::nullopt || node2.piece == std::nullopt) {
                    return false;
                }
                break;
            }
            case 1: {
                const Node& node1 = nodes[0];
                const Node& node2 = nodes[2];
                const Node& node3 = nodes[4];
                if (node1.piece == std::nullopt || node2.piece == std::nullopt
                        || node3.piece == std::nullopt)
                    return false;
                break;
            }
            case 2: {
                const Node& node1 = nodes[1];
                const Node& node2 = nodes[14];
                if (node1.piece == std::nullopt || node2.piece == std::nullopt)
                    return false;
                break;
            }
            case 3: {
                const Node& node1 = nodes[4];
                const Node& node2 = nodes[10];
                if (node1.piece == std::nullopt || node2.piece == std::nullopt)
                    return false;
                break;
            }
            case 4: {
                const Node& node1 = nodes[1];
                const Node& node2 = nodes[3];
                const Node& node3 = nodes[5];
                const Node& node4 = nodes[7];
                if (node1.piece == std::nullopt || node2.piece == std::nullopt
                        || node3.piece == std::nullopt || node4.piece == std::nullopt)
                    return false;
                break;
            }
            case 5: {
                const Node& node1 = nodes[4];
                const Node& node2 = nodes[13];
                if (node1.piece == std::nullopt || node2.piece == std::nullopt)
                    return false;
                break;
            }
            case 6: {
                const Node& node1 = nodes[7];
                const Node& node2 = nodes[11];
                if (node1.piece == std::nullopt || node2.piece == std::nullopt)
                    return false;
                break;
            }
            case 7: {
                const Node& node1 = nodes[4];
                const Node& node2 = nodes[6];
                const Node& node3 = nodes[8];
                if (node1.piece == std::nullopt || node2.piece == std::nullopt
                        || node3.piece == std::nullopt)
                    return false;
                break;
            }
            case 8: {
                const Node& node1 = nodes[7];
                const Node& node2 = nodes[12];
                if (node1.piece == std::nullopt || node2.piece == std::nullopt)
                    return false;
                break;
            }
            case 9: {
                const Node& node1 = nodes[0];
                const Node& node2 = nodes[10];
                const Node& node3 = nodes[21];
                if (node1.piece == std::nullopt || node2.piece == std::nullopt
                        || node3.piece == std::nullopt)
                    return false;
                break;
            }
            case 10: {
                const Node& node1 = nodes[3];
                const Node& node2 = nodes[9];
                const Node& node3 = nodes[11];
                const Node& node4 = nodes[18];
                if (node1.piece == std::nullopt || node2.piece == std::nullopt
                        || node3.piece == std::nullopt || node4.piece == std::nullopt)
                    return false;
                break;
            }
            case 11: {
                const Node& node1 = nodes[6];
                const Node& node2 = nodes[10];
                const Node& node3 = nodes[15];
                if (node1.piece == std::nullopt || node2.piece == std::nullopt
                        || node3.piece == std::nullopt)
                    return false;
                break;
            }
            case 12: {
                const Node& node1 = nodes[8];
                const Node& node2 = nodes[13];
                const Node& node3 = nodes[17];
                if (node1.piece == std::nullopt || node2.piece == std::nullopt
                        || node3.piece == std::nullopt)
                    return false;
                break;
            }
            case 13: {
                const Node& node1 = nodes[5];
                const Node& node2 = nodes[12];
                const Node& node3 = nodes[14];
                const Node& node4 = nodes[20];
                if (node1.piece == std::nullopt || node2.piece == std::nullopt
                        || node3.piece == std::nullopt || node4.piece == std::nullopt)
                    return false;
                break;
            }
            case 14: {
                const Node& node1 = nodes[2];
                const Node& node2 = nodes[13];
                const Node& node3 = nodes[23];
                if (node1.piece == std::nullopt || node2.piece == std::nullopt
                        || node3.piece == std::nullopt)
                    return false;
                break;
            }
            case 15: {
                const Node& node1 = nodes[11];
                const Node& node2 = nodes[16];
                if (node1.piece == std::nullopt || node2.piece == std::nullopt)
                    return false;
                break;
            }
            case 16: {
                const Node& node1 = nodes[15];
                const Node& node2 = nodes[17];
                const Node& node3 = nodes[19];
                if (node1.piece == std::nullopt || node2.piece == std::nullopt
                        || node3.piece == std::nullopt)
                    return false;
                break;
            }
            case 17: {
                const Node& node1 = nodes[12];
                const Node& node2 = nodes[16];
                if (node1.piece == std::nullopt || node2.piece == std::nullopt)
                    return false;
                break;
            }
            case 18: {
                const Node& node1 = nodes[10];
                const Node& node2 = nodes[19];
                if (node1.piece == std::nullopt || node2.piece == std::nullopt)
                    return false;
                break;
            }
            case 19: {
                const Node& node1 = nodes[16];
                const Node& node2 = nodes[18];
                const Node& node3 = nodes[20];
                const Node& node4 = nodes[22];
                if (node1.piece == std::nullopt || node2.piece == std::nullopt
                        || node3.piece == std::nullopt || node4.piece == std::nullopt)
                    return false;
                break;
            }
            case 20: {
                const Node& node1 = nodes[13];
                const Node& node2 = nodes[19];
                if (node1.piece == std::nullopt || node2.piece == std::nullopt)
                    return false;
                break;
            }
            case 21: {
                const Node& node1 = nodes[9];
                const Node& node2 = nodes[22];
                if (node1.piece == std::nullopt || node2.piece == std::nullopt)
                    return false;
                break;
            }
            case 22: {
                const Node& node1 = nodes[19];
                const Node& node2 = nodes[21];
                const Node& node3 = nodes[23];
                if (node1.piece == std::nullopt || node2.piece == std::nullopt
                        || node3.piece == std::nullopt)
                    return false;
                break;
            }
            case 23: {
                const Node& node1 = nodes[14];
                const Node& node2 = nodes[22];
                if (node1.piece == std::nullopt || node2.piece == std::nullopt)
                    return false;
                break;
            }
        }
    }

    return true;
}

bool GamePlay::is_phase_two() {
    return plies >= 18;
}

void GamePlay::game_over(Ending ending) {
    phase = GamePhase::GameOver;
    this->ending = ending;
    std::cout << "Game over: " << static_cast<int>(ending) << '\n';
}

void GamePlay::phase_two() {
    phase = GamePhase::MovePieces;
    std::cout << "Phase two\n";
}

bool GamePlay::threefold_repetition() {
    ThreefoldRepetition::Position current;

    current.turn = turn;

    for (std::size_t i = 0; i < nodes.size(); i++) {
        if (nodes[i].piece.has_value()) {
            current.nodes[i] = (
                nodes[i].piece->player == Player::White
                ?
                ThreefoldRepetition::Node::White
                :
                ThreefoldRepetition::Node::Black
            );
        } else {
            current.nodes[i] = ThreefoldRepetition::Node::Empty;
        }
    }

    for (const ThreefoldRepetition::Position& position : repetition.twos) {
        if (position == current) {
            std::cout << "Threefold repetition\n";

            return true;
        }
    }

    const auto& list = repetition.ones;
    for (auto iter_before = list.cbefore_begin(), iter = list.cbegin(); iter != list.cend(); iter_before++, iter++) {
        if (*iter == current) {
            repetition.ones.erase_after(iter_before);
            repetition.twos.push_front(current);

            return false;
        }
    }

    repetition.ones.push_front(current);

    return false;
}

void GamePlay::clear_repetition() {
    repetition.ones.clear();
    repetition.twos.clear();
}

void GamePlay::record_place_move(int node_index) {
    muhle::Move move;
    move.type = muhle::MoveType::Place;
    move.place.node_index = node_index;

    const muhle::Player player = (
        turn == Player::White ? muhle::Player::White : muhle::Player::Black
    );

    log.current_move.move = move;
    log.current_move.player = player;
}

void GamePlay::record_move_move(int node_source_index, int node_destination_index) {
    muhle::Move move;
    move.type = muhle::MoveType::Move;
    move.move.node_source_index = node_source_index;
    move.move.node_destination_index = node_destination_index;

    const muhle::Player player = (
        turn == Player::White ? muhle::Player::White : muhle::Player::Black
    );

    log.current_move.move = move;
    log.current_move.player = player;
}

void GamePlay::record_take_move(int node_index) {
    switch (log.current_move.move.type) {
        case muhle::MoveType::Place:
            log.current_move.move.type = muhle::MoveType::PlaceTake;
            log.current_move.move.place_take.node_index = log.current_move.move.place.node_index;
            log.current_move.move.place_take.node_take_index = node_index;

            break;
        case muhle::MoveType::Move: {
            log.current_move.move.type = muhle::MoveType::MoveTake;
            const muhle::Idx node_source_index = log.current_move.move.move.node_source_index;
            const muhle::Idx node_destination_index = log.current_move.move.move.node_destination_index;
            log.current_move.move.move_take.node_source_index = node_source_index;
            log.current_move.move.move_take.node_destination_index = node_destination_index;
            log.current_move.move.move_take.node_take_index = node_index;

            break;
        }
        default:
            assert(false);
            break;
    }
}

void GameTest::setup() {
    for (std::size_t i = 0; i < nodes.size(); i++) {
        nodes[i].index = static_cast<int>(i);
    }
}

void GameTest::update_nodes_positions(float board_unit, glm::vec2 board_offset) {
    for (std::size_t i = 0; i < nodes.size(); i++) {
        nodes[i].position = glm::vec2(
            NODE_POSITIONS[i][0] * board_unit + board_offset.x,
            NODE_POSITIONS[i][1] * board_unit + board_offset.y
        );

        if (nodes[i].piece.has_value()) {
            nodes[i].piece->position = nodes[i].position;
        }
    }
}

void GameTest::user_action(glm::vec2 position, MouseButton button, Player player) {
    for (Node& node : nodes) {
        if (!point_in_node(position, node)) {
            continue;
        }

        // This is the selected node

        switch (button) {
            case MouseButton::Left:
                check_add_piece(player, node.index);

                break;
            case MouseButton::Right:

                check_remove_piece(node.index);

                break;
        }

        break;
    }
}

muhle::Position GameTest::get_position() {
    return position(nodes, plies, turn);
}

void GameTest::check_add_piece(Player player, int node_index) {
    Node& node = nodes[node_index];

    if (node.piece != std::nullopt) {
        return;
    }

    if (player == Player::White) {
        if (white_pieces_on_board >= 9) {
            return;
        }

        white_pieces_on_board++;
    } else {
        if (black_pieces_on_board >= 9) {
            return;
        }

        black_pieces_on_board++;
    }

    Piece piece;
    piece.player = player;
    piece.position = node.position;

    node.piece = std::make_optional(piece);
}

void GameTest::check_remove_piece(int node_index) {
    Node& node = nodes[node_index];

    if (node.piece == std::nullopt) {
        return;
    }

    const Player player = node.piece->player;

    if (player == Player::White) {
        if (white_pieces_on_board == 0) {
            return;
        }

        white_pieces_on_board--;
    } else {
        if (black_pieces_on_board == 0) {
            return;
        }

        black_pieces_on_board--;
    }

    node.piece = std::nullopt;
}
