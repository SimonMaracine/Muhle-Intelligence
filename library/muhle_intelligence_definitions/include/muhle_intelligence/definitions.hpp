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
        Position position;
        unsigned int plies {};
        std::forward_list<Position> previous_positions;  // Push them in order from oldest to newest
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

    struct Result {
        Move result {};
        Player player {};

        double time = 0.0;
        Eval evaluation = 0;
        unsigned int positions_evaluated = 0;

        bool done = false;
    };
}
