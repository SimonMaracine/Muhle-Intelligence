#pragma once

#include <array>
#include <vector>
#include <functional>
#include <string_view>

#include <gui_base/gui_base.hpp>
#include <muhle_intelligence/definitions.hpp>

using Idx = int;

inline constexpr Idx INVALID_INDEX = -1;

enum class Player {
    White = 0,
    Black = 1
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

enum class GameOver {
    None,
    WinnerWhite,
    WinnerBlack,
    TieBetweenBothPlayers
};

struct Node {
    ImVec2 position;
    Idx index = INVALID_INDEX;
    Piece piece {};
};

struct Move {
    union {
        struct {
            Idx place_index;
        } place;

        struct {
            Idx source_index;
            Idx destination_index;
        } move;

        struct {
            Idx place_index;
            Idx take_index;
        } place_take;

        struct {
            Idx source_index;
            Idx destination_index;
            Idx take_index;
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
        std::array<Node, 24> board {};
        Player turn {};

        bool operator==(const Position& other) const {
            return board == other.board && turn == other.turn;
        }
    };

    // Check position only after changed turn
    bool check_position(const std::array<::Node, 24>& board, Player turn);
    void clear_repetition();
    const std::vector<Position>& get_positions() const { return positions; }
private:
    std::vector<Position> positions;
};

class MoveLogging {
public:
    void log_move(const Move& move, Player player);
    void clear_history();
    const std::vector<std::pair<muhle::Move, muhle::Player>>& get_history() const { return move_history; }
private:
    std::vector<std::pair<muhle::Move, muhle::Player>> move_history;
};

bool is_valid_smn(std::string_view string);
