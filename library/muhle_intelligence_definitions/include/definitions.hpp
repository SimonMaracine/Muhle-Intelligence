#pragma once

#include <array>

namespace muhle {
    inline constexpr int NODES = 24;
    inline constexpr int INVALID_INDEX = -1;

    enum class Player {
        White,
        Black
    };

    enum class Piece {  // TODO change
        Black = -1,
        None = 0,
        White = 1
    };

    enum class MoveType {
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
                int node_index;
            } place;

            struct {
                int node_source_index;
                int node_destination_index;
            } move;

            struct {
                int node_index;
                int node_take_index;
            } place_take;

            struct {
                int node_source_index;
                int node_destination_index;
                int node_take_index;
            } move_take;
        };

        MoveType type {};
        Piece piece = Piece::None;
    };

    struct Result {
        Move result {};
        double time = 0.0;
        int evaluation = 0;
        unsigned int positions_evaluated = 0;

        bool done = false;
    };
}
