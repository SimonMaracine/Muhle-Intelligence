#pragma once

#include <array>

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

    using Pieces = std::array<Piece, NODES>;

    struct Position {
        Pieces pieces {};
        Player player {};
        unsigned int plies {};
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
