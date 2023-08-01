#pragma once

#include <array>

namespace muhle {
    inline constexpr int NODES = 24;

    enum class Player {
        White,
        Black
    };

    enum class Piece {  // TODO change
        White,
        Black,
        None
    };

    enum class MoveType {
        Place,
        Move,
        PlaceTake,
        MoveTake
    };

    struct Position {
        std::array<Piece, NODES> pieces {};
        unsigned int white_pieces_outside {};
        unsigned int black_pieces_outside {};
    };

    struct Move {
        // union {
            struct {
                int node_index;
            } place {};

            struct {
                int node_source_index;
                int node_destination_index;
            } move {};

            struct {
                int node_index;
                int node_take_index;
            } place_take {};

            struct {
                int node_source_index;
                int node_destination_index;
                int node_take_index;
            } move_take {};
        // };

        MoveType type {};
        Piece piece = Piece::None;
    };

    struct Result {
        bool done = false;
        Move result {};
    };
}
