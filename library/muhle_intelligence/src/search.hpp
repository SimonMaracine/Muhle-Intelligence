#pragma once

#include <unordered_map>
#include <string>

#include <muhle_intelligence/definitions.hpp>

#include "search_context.hpp"

/*
    FIXME
    segmentation fault in the middle of a game
*/

namespace muhle {
    // For every invocation of the AI algorithm, create a new search context object
    class Search {
    public:
        void setup(std::unordered_map<std::string, int>& parameters);
        void search(const Position& position, Result& result);
    private:
        void figure_out_position(const Position& position);

        Eval minimax_w(unsigned int depth, unsigned int plies_from_root, Eval alpha, Eval beta);
        Eval minimax_b(unsigned int depth, unsigned int plies_from_root, Eval alpha, Eval beta);
        unsigned int test_moves(Player player, unsigned int depth);

        SearchCtx ctx;

        struct {
            Move move {};
            Piece piece {};
        } best_move;
        unsigned int positions_evaluated = 0;

        Parameters parameters;
    };
}
