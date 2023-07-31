#pragma once

#include <ostream>
#include <array>
#include <string_view>

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

    std::ostream& operator<<(std::ostream& stream, const Move& move);

    struct Result {
        bool done = false;
        Move result {};
    };

    struct MuhleIntelligence {
        virtual ~MuhleIntelligence() = default;

        virtual void initialize() = 0;
        virtual void search(const Position& position, Player player, Result& result) = 0;
        virtual void join_thread() = 0;
        virtual void set_parameter(std::string_view parameter, int value) = 0;
    };
}
