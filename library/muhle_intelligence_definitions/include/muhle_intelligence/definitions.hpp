#pragma once

#include <array>
#include <cstdint>

namespace muhle {
    using Idx = std::int_fast8_t;
    using IterIdx = int;

    inline constexpr IterIdx NODES = 24;
    inline constexpr Idx INVALID_INDEX = -1;

    using Eval = int;

    enum class Player {
        White,
        Black
    };

    enum class Piece : Idx {
        Black = -1,
        None = 0,
        White = 1
    };

    enum class MoveType : std::int_fast8_t {
        Place,
        Move,
        PlaceTake,
        MoveTake
    };

    struct Position {
        std::array<Piece, NODES> pieces {};
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
