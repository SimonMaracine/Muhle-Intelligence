#pragma once

#include <array>

#include <muhle_intelligence/definitions.hpp>

#include "muhle_intelligence/internal/search_context.hpp"

namespace muhle {
    bool all_pieces_in_mills(SearchCtx& ctx, Piece piece);
    std::array<Idx, 5> neighbor_free_positions(SearchCtx& ctx, Idx index);
    bool is_mill(SearchCtx& ctx, Piece piece, Idx index);
    bool is_game_over(SearchCtx& ctx, Eval& evaluation_game_over);
    unsigned int pieces_on_board(SearchCtx& ctx, Piece piece);
    Piece opponent_piece(Piece type);
}
