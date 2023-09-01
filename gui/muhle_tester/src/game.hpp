#pragma once

#include <optional>
#include <array>
#include <forward_list>
#include <functional>

#include <glm/glm.hpp>
#include <muhle_intelligence/definitions.hpp>

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

struct MoveLogging {
    using ChangeTurnCallback = std::function<void(muhle::Move, muhle::Player)>;

    ChangeTurnCallback callback = nullptr;

    struct {
        muhle::Move move;
        muhle::Player player;
    } current_move;
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
    MoveLogging log;

    void setup(MoveLogging::ChangeTurnCallback callback);
    void update_nodes_positions(float board_unit, glm::vec2 board_offset);
    void user_action(glm::vec2 position);
    muhle::Position get_position();

    const char* player_to_string();
    const char* phase_to_string();
    const char* ending_to_string();

    void place_piece(int node_index);
    void move_piece(int node_source_index, int node_destination_index);
    void take_piece(int node_index);

    void check_select_piece(glm::vec2 position);
    void check_place_piece(glm::vec2 position);
    void check_move_piece(glm::vec2 position);
    void check_take_piece(glm::vec2 position);

    unsigned int change_turn();
    Player opponent(Player player);
    bool can_potentially_move(Node& node_src, Node& node_dest);
    bool piece_in_mill(const Node& node, Player player);
    unsigned int pieces_in_mills(Player player);
    bool player_has_two_pieces(Player player);
    bool player_has_three_pieces(Player player);
    bool player_has_no_legal_moves(Player player);
    bool is_phase_two();
    void game_over(Ending ending);
    void phase_two();
    bool threefold_repetition();
    void clear_repetition();

    void record_place_move(int node_index);
    void record_move_move(int node_source_index, int node_destination_index);
    void record_take_move(int node_index);
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
    void user_action(glm::vec2 position, MouseButton button, Player player = Player::White);
    muhle::Position get_position();

    void check_add_piece(Player player, int node_index);
    void check_remove_piece(int node_index);
};
