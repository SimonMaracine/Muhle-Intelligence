#pragma once

#include <vector>

#include <muhle_intelligence/definitions.hpp>

#include "muhle_intelligence/internal/search_context.hpp"
#include "muhle_intelligence/internal/array.hpp"

namespace muhle {
    bool all_pieces_in_mills(const Board& board, Piece piece);
    Array<Idx, 4> neighbor_free_positions(const Board& board, Idx index);
    bool is_mill(const Board& board, Piece piece, Idx index);
    bool is_game_over(const SearchNode& node, Eval& evaluation_game_over);
    unsigned int pieces_on_board(const SearchNode& node, Piece piece);
    Player opponent(Player player);
    Piece opponent_piece(Piece piece);
    void count_pieces(const Board& board, unsigned int& white, unsigned int& black);
}
