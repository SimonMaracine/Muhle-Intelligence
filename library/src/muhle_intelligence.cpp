#include <ostream>

#include "muhle_intelligence/muhle_intelligence.hpp"

namespace muhle {
    Move create_place(Piece piece, int node_index) {
        Move move;
        move.place.node_index = node_index;
        move.type = MoveType::Place;
        move.piece = piece;

        return move;
    }

    Move create_move(Piece piece, int node_source_index, int node_destination_index) {
        Move move;
        move.move.node_source_index = node_source_index;
        move.move.node_destination_index = node_destination_index;
        move.type = MoveType::Move;
        move.piece = piece;

        return move;
    }

    Move create_place_take(Piece piece, int node_index, int node_take_index) {
        Move move;
        move.place_take.node_index = node_index;
        move.place_take.node_take_index = node_take_index;
        move.type = MoveType::PlaceTake;
        move.piece = piece;

        return move;
    }

    Move create_move_take(Piece piece, int node_source_index, int node_destination_index, int node_take_index) {
        Move move;
        move.move_take.node_source_index = node_source_index;
        move.move_take.node_destination_index = node_destination_index;
        move.move_take.node_take_index = node_take_index;
        move.type = MoveType::MoveTake;
        move.piece = piece;

        return move;
    }

    std::ostream& operator<<(std::ostream& stream, const Move& move) {
        switch (move.type) {
            case MoveType::Place:
                stream << "[Place: " << move.place.node_index << ']';
                break;
            case MoveType::Move:
                stream << "[Move: " << move.move.node_source_index << " -> "
                    << move.move.node_destination_index << ']';
                break;
            case MoveType::PlaceTake:
                stream << "[PlaceTake: " << move.place_take.node_index << "; "
                    << move.place_take.node_take_index << ']';
                break;
            case MoveType::MoveTake:
                stream << "[MoveTake: " << move.move_take.node_source_index << " -> "
                    << move.move_take.node_destination_index << "; " << move.move_take.node_take_index << ']';
                break;
        }

        return stream;
    }
}
