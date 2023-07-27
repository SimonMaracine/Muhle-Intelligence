#pragma once

#include <optional>
#include <array>

#include <glm/glm.hpp>

inline constexpr float NODE_RADIUS = 25.0f;
inline constexpr int INVALID_INDEX = -1;

enum class Player {
    White,
    Black
};

enum class GamePhase {
    PlacePieces = 1,
    MovePieces,
    GameOver
};

enum class Ending {
    None,
    WinnerWhite,
    WinnerBlack,
    TieBetweenBothPlayers
};

struct Piece {
    glm::vec2 position {};

    Player type {};
};

struct Node {
    glm::vec2 position {};

    std::optional<Piece> piece;

    int index = INVALID_INDEX;
};

struct Game {
    unsigned int white_pieces_on_board = 0;
    unsigned int black_pieces_on_board = 0;
    unsigned int white_pieces_outside = 9;
    unsigned int black_pieces_outside = 9;

    Player turn = Player::White;
    GamePhase phase = GamePhase::PlacePieces;
    Ending ending = Ending::None;

    std::array<Node, 24> nodes {};

    int selected_piece_index = INVALID_INDEX;

    std::array<bool, 2> can_jump {};

    bool must_take_piece = false;

    void setup();
    void update_nodes_positions(float board_unit, glm::vec2 board_offset);
    void user_click(glm::vec2 mouse_position);

    void place_piece(Player type, Node& node);
    void move_piece(Node& node_src, Node& node_dest);
    void take_piece(Node& node);

    void check_select_piece(glm::vec2 mouse_position);
    void check_place_piece(glm::vec2 mouse_position);
    void check_move_piece(glm::vec2 mouse_position);
    void check_take_piece(glm::vec2 mouse_position);

    bool point_in_node(glm::vec2 mouse_position, const Node& node);
    void change_turn();
    Player opponent(Player player);
    bool can_potentially_move(Node& node_src, Node& node_dest);
    bool piece_in_mill(const Node& node, Player type);
    unsigned int pieces_in_mills(Player type);
    bool player_has_two_pieces(Player type);
    bool player_has_three_pieces(Player type);
    bool player_has_no_legal_moves(Player type);
    void game_over(Ending ending);
};
