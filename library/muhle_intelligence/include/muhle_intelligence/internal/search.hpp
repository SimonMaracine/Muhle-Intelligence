#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include <muhle_intelligence/definitions.hpp>

#include "muhle_intelligence/internal/search_context.hpp"

/*
    FIXME
    finish implementing threefold repetition (correctly pass and store previous positions, cut irrelevant positions)
    change library API
    implement 50 move rule
*/

namespace muhle {
    // For every invocation of the AI algorithm, create a new search instance object
    class Search {
    public:
        using Iter = std::vector<Position>::const_iterator;

        void setup(std::unordered_map<std::string, int>& parameters);
        Move search(const SmnPosition& position, Iter prev_positions_begin, Iter prev_positions_end, Result& result);
    private:
        void setup_position(const SmnPosition& position, Iter prev_positions_begin, Iter prev_positions_end);

        // Pass null to previous_node, if there should be no history
        Eval minimax(Player player, unsigned int depth, unsigned int plies_from_root,
            Eval alpha, Eval beta, const repetition::Node* previous_node);

        SearchCtx ctx;

        struct {
            Move move {};
            Piece piece {};
        } best_move;
        unsigned int positions_evaluated {0};

        Parameters parameters;
    };
}
