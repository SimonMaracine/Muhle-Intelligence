#include <iostream>
#include <cassert>
#include <algorithm>

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

void Game::setup() {
    Piece piece1;
    piece1.type = PieceType::Black;
    piece1.position.x = nodes[0].position.x;
    piece1.position.y = nodes[0].position.y;

    nodes[0].piece = piece1;

    Piece piece2;
    piece2.type = PieceType::White;
    piece2.position.x = nodes[1].position.x;
    piece2.position.y = nodes[1].position.y;

    nodes[1].piece = piece2;
}

void Game::update_nodes_positions(float board_unit, glm::vec2 board_offset) {
    for (size_t i = 0; i < nodes.size(); i++) {
        nodes[i].position = glm::vec2(
            NODE_POSITIONS[i][0] * board_unit + board_offset.x,
            NODE_POSITIONS[i][1] * board_unit + board_offset.y
        );

        if (nodes[i].piece.has_value()) {
            nodes[i].piece.value().position = nodes[i].position;
        }
    }
}

void Game::user_click(glm::vec2 mouse_position) {
    switch (phase) {
        case GamePhase::PlacePieces:
            check_place_piece(mouse_position);
            break;
        case GamePhase::MovePieces:
            check_select_piece(mouse_position);
            check_move_piece(mouse_position);
            break;
    }
}

void Game::place_piece(PieceType type, Node& node) {
    assert(node.piece == std::nullopt);

    Piece piece;
    piece.type = type;
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

        if (i == selected_piece_index) {
            selected_piece_index = INVALID_INDEX;
            break;
        }

        if (node.piece == std::nullopt) {
            continue;
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

        if (node.piece != std::nullopt) {
            continue;
        }

        // Placing a piece

        if (turn == Player::White) {
            place_piece(PieceType::White, node);
            white_pieces_on_board++;
            white_pieces_outside--;
        } else {
            place_piece(PieceType::Black, node);
            black_pieces_on_board++;
            black_pieces_outside--;
        }

        change_turn();

        if (white_pieces_outside + black_pieces_outside == 0) {
            phase = GamePhase::MovePieces;
            std::cout << "Phase two\n";
        }

        // TODO other

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

        if (node_dest.piece != std::nullopt) {
            continue;
        }

        // Moving a piece

        move_piece(node_src, node_dest);

        change_turn();

        selected_piece_index = INVALID_INDEX;

        // TODO other

        break;
    }
}

void Game::check_take_piece(glm::vec2 mouse_position) {

}

bool Game::point_in_node(glm::vec2 mouse_position, const Node& node) {
    glm::vec2 sub = node.position - mouse_position;

    return glm::length(sub) < NODE_RADIUS;
}

void Game::change_turn() {
    if (turn == Player::White) {
        turn = Player::Black;
    } else {
        turn = Player::White;
    }
}
