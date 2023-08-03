#pragma once

#include <optional>
#include <array>
#include <forward_list>
#include <functional>

#include <glm/glm.hpp>

inline constexpr float NODE_RADIUS = 25.0f;
inline constexpr int INVALID_INDEX = -1;
inline constexpr unsigned int MAX_PLIES_WITHOUT_MILLS = 50;

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

    Player player {};
};

struct Node {
    glm::vec2 position {};

    std::optional<Piece> piece;

    int index = INVALID_INDEX;
};

struct ThreefoldRepetition {
    enum class Node {
        Empty,
        White,
        Black
    };

    struct Position {
        std::array<Node, 24> nodes {};
        Player turn {};

        bool operator==(const Position& other) const {
            return nodes == other.nodes && turn == other.turn;
        }
    };

    std::forward_list<Position> ones;
    std::forward_list<Position> twos;
};

struct GamePlay {
    unsigned int white_pieces_on_board = 0;
    unsigned int white_pieces_outside = 9;
    unsigned int black_pieces_on_board = 0;
    unsigned int black_pieces_outside = 9;

    Player turn = Player::White;
    GamePhase phase = GamePhase::PlacePieces;
    Ending ending = Ending::None;
    unsigned int plies = 0;

    std::array<Node, 24> nodes {};

    int selected_piece_index = INVALID_INDEX;

    std::array<bool, 2> can_jump { false, false };

    bool must_take_piece = false;

    unsigned int plies_without_mills = 0;

    ThreefoldRepetition repetition;

    using ChangedTurn = std::function<void()>;
    ChangedTurn changed_turn;

    void setup(ChangedTurn changed_turn);
    void update_nodes_positions(float board_unit, glm::vec2 board_offset);
    void user_click(glm::vec2 mouse_position);
    std::array<int, 24> get_position();

    void place_piece(int node_index);
    void move_piece(int node_source_index, int node_destination_index);
    void take_piece(int node_index);

    void check_select_piece(glm::vec2 mouse_position);
    void check_place_piece(glm::vec2 mouse_position);
    void check_move_piece(glm::vec2 mouse_position);
    void check_take_piece(glm::vec2 mouse_position);

    unsigned int change_turn();
    Player opponent(Player player);
    bool can_potentially_move(Node& node_src, Node& node_dest);
    bool piece_in_mill(const Node& node, Player player);
    unsigned int pieces_in_mills(Player player);
    bool player_has_two_pieces(Player player);
    bool player_has_three_pieces(Player player);
    bool player_has_no_legal_moves(Player player);
    void game_over(Ending ending);
    bool threefold_repetition();
    void clear_repetition();
};

struct GameTest {
    unsigned int white_pieces_on_board = 0;
    unsigned int black_pieces_on_board = 0;

    Player turn = Player::White;
    unsigned int plies = 0;

    std::array<Node, 24> nodes {};

    enum class MouseButton {
        Left,
        Right
    };

    void setup();
    void update_nodes_positions(float board_unit, glm::vec2 board_offset);
    void user_click(glm::vec2 mouse_position, MouseButton button, Player player = Player::White);
    std::array<int, 24> get_position();
    void set_pieces_outside(Player player, int change);

    void check_add_piece(Player player, int node_index);
    void check_remove_piece(int node_index);
};
