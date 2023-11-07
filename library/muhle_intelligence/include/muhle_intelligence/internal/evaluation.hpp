#pragma once

#include <muhle_intelligence/definitions.hpp>

#include "muhle_intelligence/internal/search_context.hpp"

namespace muhle {
    // At three pieces, a player will have this freedom value, it simplifies things a bit
    inline constexpr Eval THREE_PIECES_FREEDOM {36};

    Eval evaluate_position(
        const SearchNode& node,
        const Parameters& parameters,
        Eval evaluation_game_over,
        unsigned int plies_from_root,
        unsigned int& positions_evaluated
    );
    Eval calculate_material_both(const SearchNode& node);
    Eval calculate_freedom_both(const SearchNode& node);
    void get_players_freedom(const SearchNode& node, unsigned int& white, unsigned int& black);
    unsigned int get_piece_freedom(const SearchNode& node, Idx index);
}
