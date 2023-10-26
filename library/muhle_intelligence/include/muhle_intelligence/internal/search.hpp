#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include <muhle_intelligence/definitions.hpp>

#include "muhle_intelligence/internal/search_context.hpp"

/*
    FIXME
    finish implementing threefold repetition (correctly pass and store previous positions, cut irrelevant positions)
    implement 50 move rule
*/

namespace muhle {
    // For every invocation of the AI algorithm, create a new search instance object
    class Search {
    public:
        void setup(std::unordered_map<std::string, int>& parameters);
        void search(const SmnPosition& position, const std::vector<Position>& previous_positions, Result& result);
    private:
        void setup_position(const SmnPosition& position, const std::vector<Position>& previous_positions);

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
