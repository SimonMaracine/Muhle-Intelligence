#pragma once

#include <cstddef>

#include <muhle_intelligence/definitions.hpp>

#include "muhle_intelligence/internal/search_context.hpp"
#include "muhle_intelligence/internal/array.hpp"

namespace muhle {
    // Some thinking concludes that there cannot be more than 58 moves in a ply
    inline constexpr std::size_t MAX_MOVES = 58;

    void make_move(SearchCtx& ctx, const Move& move, Piece piece);
    void unmake_move(SearchCtx& ctx, const Move& move, Piece piece);
    void generate_moves(SearchCtx& ctx, Piece piece, Array<Move, MAX_MOVES>& moves);
    Move random_move(SearchCtx& ctx, Piece piece);
}
