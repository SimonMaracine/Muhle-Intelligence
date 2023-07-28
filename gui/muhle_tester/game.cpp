#include <iostream>
#include <cassert>
#include <algorithm>
#include <cstddef>

#include <glm/glm.hpp>

#include "game.hpp"

static const int NODE_POSITIONS[24][2] = {
    { 1, 1 },
    { 4, 1 },
    { 7, 1 },
    { 2, 2 },
    { 4, 2 },
    { 6, 2 },
    { 3, 3 },
    { 4, 3 },
    { 5, 3 },
    { 1, 4 },
    { 2, 4 },
    { 3, 4 },
    { 5, 4 },
    { 6, 4 },
    { 7, 4 },
    { 3, 5 },
    { 4, 5 },
    { 5, 5 },
    { 2, 6 },
    { 4, 6 },
    { 6, 6 },
    { 1, 7 },
    { 4, 7 },
    { 7, 7 }
};

static const unsigned int MILLS_COUNT = 16;
static const unsigned int MILLS[MILLS_COUNT][3] = {
    { 0, 1, 2 }, { 2, 14, 23 }, { 21, 22, 23 }, { 0, 9, 21 },
    { 3, 4, 5 }, { 5, 13, 20 }, { 18, 19, 20 }, { 3, 10, 18 },
    { 6, 7, 8 }, { 8, 12, 17 }, { 15, 16, 17 }, { 6, 11, 15 },
    { 1, 4, 7 }, { 12, 13, 14 }, { 16, 19, 22 }, { 9, 10, 11 }
};

void Game::setup() {
    for (size_t i = 0; i < nodes.size(); i++) {
        nodes[i].index = i;
    }
}

void Game::update_nodes_positions(float board_unit, glm::vec2 board_offset) {
    for (size_t i = 0; i < nodes.size(); i++) {
        nodes[i].position = glm::vec2(
            NODE_POSITIONS[i][0] * board_unit + board_offset.x,
            NODE_POSITIONS[i][1] * board_unit + board_offset.y
        );

        if (nodes[i].piece.has_value()) {
            nodes[i].piece->position = nodes[i].position;
        }
    }
}

void Game::user_click(glm::vec2 mouse_position) {
    switch (phase) {
        case GamePhase::PlacePieces:
            if (must_take_piece) {
                check_take_piece(mouse_position);
            } else {
                check_place_piece(mouse_position);
            }

            break;
        case GamePhase::MovePieces:
            if (must_take_piece) {
                check_take_piece(mouse_position);
            } else {
                check_select_piece(mouse_position);
                check_move_piece(mouse_position);
            }

            break;
        case GamePhase::GameOver:
            // Do nothing

            break;
    }
}

void Game::place_piece(Player player, Node& node) {
    assert(node.piece == std::nullopt);

    Piece piece;
    piece.player = player;
    piece.position = node.position;

    node.piece = std::make_optional(piece);

    std::cout << "Placed piece\n";
}

void Game::move_piece(Node& node_src, Node& node_dest) {
    assert(node_src.piece != std::nullopt);
    assert(node_dest.piece == std::nullopt);

    std::swap(node_src.piece, node_dest.piece);

    std::cout << "Moved piece\n";
}

void Game::take_piece(Node& node) {
    assert(node.piece != std::nullopt);

    node.piece = std::nullopt;

    std::cout << "Took piece\n";
}

void Game::check_select_piece(glm::vec2 mouse_position) {
    for (size_t i = 0; i < nodes.size(); i++) {
        const Node& node = nodes[i];

        if (!point_in_node(mouse_position, node)) {
            continue;
        }

        // This is the hovered node

        if (i == selected_piece_index) {
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

        selected_piece_index = i;

        std::cout << "Selected piece index " << i << '\n';

        break;
    }
}

void Game::check_place_piece(glm::vec2 mouse_position) {
    for (Node& node : nodes) {
        if (!point_in_node(mouse_position, node)) {
            continue;
        }

        // This is the hovered node

        if (node.piece != std::nullopt) {
            break;
        }

        // Placing a piece

        if (turn == Player::White) {
            place_piece(Player::White, node);
            white_pieces_on_board++;
            white_pieces_outside--;
        } else {
            place_piece(Player::Black, node);
            black_pieces_on_board++;
            black_pieces_outside--;
        }

        // Check for mills
        if (piece_in_mill(node, turn)) {
            must_take_piece = true;
            std::cout << "Must take piece\n";
            break;
        }

        if (change_turn() == MAX_PLIES_WITHOUT_MILLS) {
            game_over(Ending::TieBetweenBothPlayers);
            break;
        }

        // Check game over; only by blocking
        if (player_has_no_legal_moves(turn)) {
            game_over(turn == Player::White ? Ending::WinnerBlack : Ending::WinnerWhite);
            break;
        }

        if (white_pieces_outside + black_pieces_outside == 0) {
            phase = GamePhase::MovePieces;
            std::cout << "Phase two\n";

            // Remember this position; now threefold repetition rule can apply
            threefold_repetition();
        }

        break;
    }
}

void Game::check_move_piece(glm::vec2 mouse_position) {
    if (selected_piece_index == INVALID_INDEX) {
        return;
    }

    Node& node_src = nodes[selected_piece_index];

    for (Node& node_dest : nodes) {
        if (!point_in_node(mouse_position, node_dest)) {
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

        // Just reset this
        selected_piece_index = INVALID_INDEX;

        move_piece(node_src, node_dest);

        // Check for mills
        if (piece_in_mill(node_dest, turn)) {
            must_take_piece = true;
            std::cout << "Must take piece\n";
            break;
        }

        if (change_turn() == MAX_PLIES_WITHOUT_MILLS) {
            game_over(Ending::TieBetweenBothPlayers);
            break;
        }

        // Check game over by repetition
        if (threefold_repetition()) {
            game_over(Ending::TieBetweenBothPlayers);
            break;
        }

        // Check game over by blocking
        if (player_has_no_legal_moves(turn)) {
            game_over(turn == Player::White ? Ending::WinnerBlack : Ending::WinnerWhite);
            break;
        }

        break;
    }
}

void Game::check_take_piece(glm::vec2 mouse_position) {
    assert(must_take_piece);

    for (Node& node : nodes) {
        if (!point_in_node(mouse_position, node)) {
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

        // If a piece in mill and not all pieces on the board are in mills
        const Player player = opponent(turn);
        unsigned int& player_pieces = (
            player == Player::White ? white_pieces_on_board : black_pieces_on_board
        );

        if (piece_in_mill(node, player) && pieces_in_mills(player) != player_pieces) {
            break;
        }

        // Taking a piece

        take_piece(node);

        player_pieces--;
        must_take_piece = false;

        if (change_turn() == MAX_PLIES_WITHOUT_MILLS) {
            game_over(Ending::TieBetweenBothPlayers);
            break;
        }

        // Must reset counter here
        plies_without_mills = 0;

        // Check jumping
        if (player_has_three_pieces(turn)) {
            can_jump[static_cast<size_t>(turn)] = true;
        }

        // Check game over
        if (player_has_two_pieces(turn) || player_has_no_legal_moves(turn)) {
            game_over(turn == Player::White ? Ending::WinnerBlack : Ending::WinnerWhite);
            break;
        }

        // Previous positions can occur no more
        clear_repetition();

        break;
    }
}

bool Game::point_in_node(glm::vec2 mouse_position, const Node& node) {
    const glm::vec2 sub = node.position - mouse_position;

    return glm::length(sub) < NODE_RADIUS;
}

unsigned int Game::change_turn() {
    turn = opponent(turn);

    plies_without_mills++;

    return plies_without_mills;
}

Player Game::opponent(Player player) {
    if (turn == Player::White) {
        return Player::Black;
    } else {
        return Player::White;
    }
}

bool Game::can_potentially_move(Node& node_src, Node& node_dest) {
    assert(node_src.piece != std::nullopt);
    assert(node_dest.piece == std::nullopt);

    if (can_jump[static_cast<size_t>(turn)]) {
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

bool Game::piece_in_mill(const Node& node, Player player) {
    assert(node.piece != std::nullopt);
    assert(node.piece->player == player);

    for (unsigned int i = 0; i < MILLS_COUNT; i++) {
        const unsigned int* mill = MILLS[i];

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

unsigned int Game::pieces_in_mills(Player player) {
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

bool Game::player_has_two_pieces(Player player) {
    if (player == Player::White) {
        return white_pieces_on_board + white_pieces_outside == 2;
    } else {
        return black_pieces_on_board + black_pieces_outside == 2;
    }
}

bool Game::player_has_three_pieces(Player player) {
    if (player == Player::White) {
        return white_pieces_on_board + white_pieces_outside == 3;
    } else {
        return black_pieces_on_board + black_pieces_outside == 3;
    }
}

bool Game::player_has_no_legal_moves(Player player) {
    if (can_jump[static_cast<size_t>(player)]) {
        return false;
    }

    // If phase is not two
    if (white_pieces_outside > 0 || black_pieces_outside > 0) {
        return false;
    }

    for (const Node& node : nodes) {
        if (node.piece == std::nullopt) {
            continue;
        }

        if (node.piece->player != player) {
            continue;
        }

        const Piece& piece = node.piece.value();

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

void Game::game_over(Ending ending) {
    phase = GamePhase::GameOver;
    this->ending = ending;
    std::cout << "Game over: " << static_cast<int>(ending) << '\n';
}

bool Game::threefold_repetition() {
    ThreefoldRepetition::Position current;

    for (size_t i = 0; i < nodes.size(); i++) {
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

    for (const ThreefoldRepetition::Position& position : repetition.ones) {
        if (position == current) {
            repetition.ones.remove(current);
            repetition.twos.push_front(current);

            return false;
        }
    }

    repetition.ones.push_front(current);

    return false;
}

void Game::clear_repetition() {
    repetition.ones.clear();
    repetition.twos.clear();
}
