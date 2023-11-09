#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include <muhle_intelligence/definitions.hpp>

#include "muhle_intelligence/internal/search_context.hpp"

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
        SearchNode& setup_nodes(
            const SmnPosition& position,
            const std::vector<SmnPosition>& prev_positions,
            const std::vector<Move>& prev_moves
        );

        Eval minimax(
            Player player,
            unsigned int depth,
            unsigned int plies_from_root,
            Eval alpha,
            Eval beta,
            SearchNode& current_node
        );

        // Contains the current position and previous positions up until the last take move
        std::vector<SearchNode> nodes;

        struct {
            Move move {};
            Piece piece {};
        } best_move;
        unsigned int positions_evaluated {0};

        Parameters parameters;
    };
}
