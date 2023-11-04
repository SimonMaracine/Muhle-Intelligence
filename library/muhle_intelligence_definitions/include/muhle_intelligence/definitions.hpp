#pragma once

#include <array>

namespace muhle {
    using Idx = signed char;
    using IterIdx = signed char;

    inline constexpr IterIdx NODES {24};
    inline constexpr Idx INVALID_INDEX {-1};

    using Eval = int;

    enum class Player {
        White = 0,
        Black = 1
    };

    enum class Piece : signed char {
        Black = -1,
        None = 0,
        White = 1
    };

    enum class MoveType : signed char {
        Place = 0,
        Move = 1,
        PlaceTake = 2,
        MoveTake = 3
    };

    using Board = std::array<Piece, NODES>;

    struct Position {
        Board board {};
        Player player {Player::White};
    };

    struct SmnPosition {
        Position position;
        unsigned int plies {};
        // TODO plies_witout_mills
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

        double time {0.0};
        Eval evaluation {0};
        unsigned int positions_evaluated {0};

        bool done {false};
    };
}
