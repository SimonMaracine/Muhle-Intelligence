#pragma once

#include <array>
#include <vector>
#include <functional>

#include <gui_base/gui_base.hpp>
#include <muhle_intelligence/definitions.hpp>

inline constexpr float NODE_RADIUS = 22.0f;
inline constexpr int INVALID_INDEX = -1;
inline constexpr unsigned int MAX_PLIES_WITHOUT_MILLS = 50;

using Idx = int;

enum class Player {
    White,
    Black
};

enum class MoveType {
    Place,
    Move,
    PlaceTake,
    MoveTake
};

enum class Piece {
    None,
    White,
    Black
};

struct Node {
    ImVec2 position;
    int index = INVALID_INDEX;
    Piece piece {};
};

struct Move {
    union {
        struct {
            Idx node_index;
        } place;

        struct {
            Idx node_source_index;
            Idx node_destination_index;
        } move;

        struct {
            Idx node_index;
            Idx node_take_index;
        } place_take;

        struct {
            Idx node_source_index;
            Idx node_destination_index;
            Idx node_take_index;
        } move_take;
    };

    MoveType type {};
};

class ThreefoldRepetition {
public:
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

    // Check position only after changed turn
    bool check_position(const std::array<::Node, 24>& nodes, Player turn);

    void clear_repetition();

    const std::vector<Position>& get_positions() const { return positions; }
private:
    std::vector<Position> positions;
};

struct MoveLogging {
    using ChangeTurnCallback = std::function<void(muhle::Move, muhle::Player)>;

    ChangeTurnCallback callback;

    struct {
        muhle::Move move;
        muhle::Player player;
    } current_move;
};

// class GamePlay {
// public:
//     void setup(MoveLogging::ChangeTurnCallback callback);
//     void update_nodes_positions(float board_unit, ImVec2 board_offset);
//     void user_action(ImVec2 position);
//     muhle::SearchInput get_input_for_search();

//     const char* player_to_string();
//     const char* phase_to_string();
//     const char* ending_to_string();

//     void place_piece(int node_index);
//     void move_piece(int node_source_index, int node_destination_index);
//     void take_piece(int node_index);

//     unsigned int white_pieces_on_board = 0;
//     unsigned int white_pieces_outside = 9;
//     unsigned int black_pieces_on_board = 0;
//     unsigned int black_pieces_outside = 9;

//     Player turn = Player::White;
//     GamePhase phase = GamePhase::PlacePieces;
//     Ending ending = Ending::None;

//     unsigned int plies = 0;
//     std::array<Node, 24> nodes {};
//     int selected_piece_index = INVALID_INDEX;
//     std::array<bool, 2> can_jump = { false, false };
//     bool must_take_piece = false;
//     unsigned int plies_without_mills = 0;
// private:
//     void check_select_piece(ImVec2 position);
//     void check_place_piece(ImVec2 position);
//     void check_move_piece(ImVec2 position);
//     void check_take_piece(ImVec2 position);

//     unsigned int change_turn();
//     Player opponent(Player player);
//     bool can_potentially_move(Node& node_src, Node& node_dest);
//     bool piece_in_mill(const Node& node, Player player);
//     unsigned int pieces_in_mills(Player player);
//     bool player_has_two_pieces(Player player);
//     bool player_has_three_pieces(Player player);
//     bool player_has_no_legal_moves(Player player);
//     bool is_phase_two();
//     void game_over(Ending ending);
//     void phase_two();

//     void record_place_move(int node_index);
//     void record_move_move(int node_source_index, int node_destination_index);
//     void record_take_move(int node_index);

//     ThreefoldRepetition repetition;
//     MoveLogging log;
// };
