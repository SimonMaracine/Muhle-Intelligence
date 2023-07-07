#pragma once

#include <iostream>
#include <functional>
#include <atomic>
#include <array>
#include <cstddef>

static constexpr int NODES = 24;

enum class Piece {
    None,
    White,
    Black
};

enum class MoveType {
    None,
    Place,
    Move,
    PlaceTake,
    MoveTake
};

struct Move {  // TODO can use union
    int place_node_index = -1;
    int take_node_index = -1;
    int move_source_node_index = -1;
    int move_destination_node_index = -1;

    Piece piece = Piece::None;
    MoveType type = MoveType::None;

    static Move create_place(Piece piece, int place_node_index);
    static Move create_move(Piece piece, int move_source_node_index, int move_destination_node_index);
    static Move create_place_take(Piece piece, int place_node_index, int take_node_index);
    static Move create_move_take(Piece piece, int move_source_node_index, int move_destination_node_index, int take_node_index);
};

std::ostream& operator<<(std::ostream& stream, const Move& move);

struct Position {
    Piece operator[](size_t index) const;
    Piece& operator[](size_t index);
    bool operator==(const Position& other) const;

    std::array<Piece, NODES> data;
    unsigned int turns = 0;
    unsigned int white_pieces_on_board = 0;
    unsigned int black_pieces_on_board = 0;
    unsigned int white_pieces_outside = 0;
    unsigned int black_pieces_outside = 0;
};

struct MinimaxAlgorithm {
    using Algorithm = std::function<void(Position, Piece, Move&, std::atomic<bool>&)>;

    virtual void search(Position position, Piece piece, Move& result, std::atomic<bool>& running) = 0;
};
