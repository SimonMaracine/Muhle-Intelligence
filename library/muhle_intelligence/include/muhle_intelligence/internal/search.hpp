#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include <muhle_intelligence/definitions.hpp>

#include "muhle_intelligence/internal/search_context.hpp"

/*
    FIXME
    implement 50 move rule
*/

namespace muhle {
    // For every invocation of the AI algorithm, create a new search instance object
    class Search {
    public:
        void setup(std::unordered_map<std::string, int>& parameters);

        Move search(
            const SmnPosition& position,
            const std::vector<SmnPosition>& prev_positions,
            const std::vector<Move>& prev_moves,
            Result& result
        );
    private:
        void setup_nodes(
            const SmnPosition& position,
            const std::vector<SmnPosition>& prev_positions,
            const std::vector<Move>& prev_moves
        );

        // Pass null to previous_node, if there should be no history
        Eval minimax(
            Player player,
            unsigned int depth,
            unsigned int plies_from_root,
            Eval alpha,
            Eval beta,
            const SearchNode* previous_node
        );

        // SearchCtx ctx;
        std::vector<SearchNode> nodes;

        struct {
            Move move {};
            Piece piece {};
        } best_move;
        unsigned int positions_evaluated {0};

        Parameters parameters;
    };
}
