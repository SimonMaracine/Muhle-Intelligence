#include <iostream>
#include <cstddef>

#include "common.hpp"

Move Move::create_place(Piece piece, int place_node_index) {
    Move move;
    move.type = MoveType::Place;
    move.piece = piece;
    move.place_node_index = place_node_index;
    return move;
}

Move Move::create_move(Piece piece, int move_source_node_index, int move_destination_node_index) {
    Move move;
    move.type = MoveType::Move;
    move.piece = piece;
    move.move_source_node_index = move_source_node_index;
    move.move_destination_node_index = move_destination_node_index;
    return move;
}

Move Move::create_place_take(Piece piece, int place_node_index, int take_node_index) {
    Move move;
    move.type = MoveType::PlaceTake;
    move.piece = piece;
    move.place_node_index = place_node_index;
    move.take_node_index = take_node_index;
    return move;
}

Move Move::create_move_take(Piece piece, int move_source_node_index, int move_destination_node_index, int take_node_index) {
    Move move;
    move.type = MoveType::MoveTake;
    move.piece = piece;
    move.move_source_node_index = move_source_node_index;
    move.move_destination_node_index = move_destination_node_index;
    move.take_node_index = take_node_index;
    return move;
}

std::ostream& operator<<(std::ostream& stream, const Move& move) {
    switch (move.type) {
        case MoveType::None:
            stream << "[None]";
            break;
        case MoveType::Place:
            stream << "[Place: " << move.place_node_index << ']';
            break;
        case MoveType::Move:
            stream << "[Move: " << move.move_source_node_index << "->" << move.move_destination_node_index << ']';
            break;
        case MoveType::PlaceTake:
            stream << "[PlaceTake: " << move.place_node_index << "; " << move.take_node_index << ']';
            break;
        case MoveType::MoveTake:
            stream << "[MoveTake: " << move.move_source_node_index << "->" << move.move_destination_node_index << "; " << move.take_node_index << ']';
            break;
    }

    return stream;
}

Piece Position::operator[](size_t index) const {
#ifdef MUHLE_DEBUG
    return data.at(index);
#else
    return data[index];
#endif
}

Piece& Position::operator[](size_t index) {
#ifdef MUHLE_DEBUG
    return data.at(index);
#else
    return data[index];
#endif
}

bool Position::operator==(const Position& other) const {
    return data == other.data;  // Don't take into account turns and pieces
}
