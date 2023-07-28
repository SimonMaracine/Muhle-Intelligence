#pragma once

#include <cstddef>

namespace muhle {
    inline constexpr size_t NODES = 24;

    enum class Player {
        White,
        Black
    };

    enum class Piece {
        None,
        White,
        Black
    };

    enum class MoveType {
        Place,
        Move,
        PlaceTake,
        MoveTake
    };

    struct Position {
        Piece pieces[NODES];
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
    };

    struct Result {
        bool running = false;
        Move result {};
    };

    struct MuhleIntelligence {
        virtual ~MuhleIntelligence() = default;

        virtual void search(Position position, Player turn, Result& result) = 0;
    };
}
