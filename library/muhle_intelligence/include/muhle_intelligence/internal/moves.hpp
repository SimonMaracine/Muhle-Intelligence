#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>

#include <muhle_intelligence/definitions.hpp>

#include "muhle_intelligence/internal/array.hpp"

namespace muhle {
    struct SearchNode;

    // Some thinking concludes that there cannot be more than 83 moves in a ply
    inline constexpr std::size_t MAX_MOVES {83};

    void play_move(SearchNode& node, const Move& move, Piece piece);
    // void unmake_move(SearchNode& node, const Move& move, Piece piece);
    void play_move(SmnPosition& position, const Move& move);
    void generate_moves(SearchNode& node, Piece piece, Array<Move, MAX_MOVES>& moves);
    Move random_move(SearchNode& node, Piece piece);
    bool is_take_move(const Move& move);
}
