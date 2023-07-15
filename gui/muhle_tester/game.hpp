#pragma once

#include <optional>
#include <array>

#include <glm/glm.hpp>

enum class Player {
    White,
    Black
};

enum class PieceType {
    White,
    Black
};

enum class GamePhase {
    PlacePieces,
    MovePieces
};

struct Piece {
    glm::vec2 position;

    PieceType type {};
};

struct Node {
    glm::vec2 position;

    std::optional<Piece> piece;
};

inline constexpr float NODE_RADIUS = 25.0f;
inline constexpr int INVALID_INDEX = -1;

struct Game {
    unsigned int white_pieces_on_board = 0;
    unsigned int black_pieces_on_board = 0;
    unsigned int white_pieces_outside = 9;
    unsigned int black_pieces_outside = 9;

    Player turn = Player::White;
    GamePhase phase = GamePhase::PlacePieces;

    std::array<Node, 24> nodes;

    int selected_piece_index = INVALID_INDEX;

    void setup();
    void update_nodes_positions(float board_unit, glm::vec2 board_offset);
    void user_click(glm::vec2 mouse_position);

    void place_piece(PieceType type, Node& node);
    void move_piece(Node& node_src, Node& node_dest);
    void take_piece(Node& node);

    void check_select_piece(glm::vec2 mouse_position);
    void check_place_piece(glm::vec2 mouse_position);
    void check_move_piece(glm::vec2 mouse_position);
    void check_take_piece(glm::vec2 mouse_position);

    bool point_in_node(glm::vec2 mouse_position, const Node& node);
    void change_turn();
};
