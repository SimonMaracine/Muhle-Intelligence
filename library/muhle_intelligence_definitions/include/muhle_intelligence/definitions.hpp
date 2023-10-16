#pragma once

#include <array>
#include <forward_list>

namespace muhle {
    using Idx = signed char;
    using IterIdx = unsigned int;

    inline constexpr IterIdx NODES = 24;
    inline constexpr Idx INVALID_INDEX = -1;

    using Eval = int;

    enum class Player {
        White,
        Black
    };

    enum class Piece : signed char {
        Black = -1,
        None = 0,
        White = 1
    };

    enum class MoveType : signed char {
        Place,
        Move,
        PlaceTake,
        MoveTake
    };

    using Board = std::array<Piece, NODES>;

    struct Position {
        Board board {};
        Player player {};
    };

    // This is passed to the search instance
    // The AI needs to know, of course, the current position to search, the number of plies of
    // the game and the previous positions
    // Technically, not all previous positions are needed for the AI to know
    struct SearchInput {
        Position current_position;
        unsigned int plies {};
        std::forward_list<Position> previous_positions;  // Push them in order from oldest to newest
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

    struct Result {
        Move result {};
        Player player {};

        double time = 0.0;
        Eval evaluation = 0;
        unsigned int positions_evaluated = 0;

        bool done = false;
    };
}
