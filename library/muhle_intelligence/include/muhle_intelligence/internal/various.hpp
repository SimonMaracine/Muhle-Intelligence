#pragma once

#include <vector>

#include <muhle_intelligence/definitions.hpp>

#include "muhle_intelligence/internal/search_context.hpp"
#include "muhle_intelligence/internal/array.hpp"

namespace muhle {
    bool all_pieces_in_mills(SearchCtx& ctx, Piece piece);
    Array<Idx, 4> neighbor_free_positions(SearchCtx& ctx, Idx index);
    bool is_mill(SearchCtx& ctx, Piece piece, Idx index);
    bool is_game_over(SearchCtx& ctx, Eval& evaluation_game_over);
    unsigned int pieces_on_board(SearchCtx& ctx, Piece piece);
    Player opponent(Player player);
    Piece opponent_piece(Piece piece);
}
