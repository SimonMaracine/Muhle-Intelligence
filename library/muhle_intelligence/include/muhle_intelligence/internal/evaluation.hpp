#pragma once

#include <muhle_intelligence/definitions.hpp>

#include "muhle_intelligence/internal/search_context.hpp"

namespace muhle {
    // At three pieces, a player will have this freedom value, it simplifies things a bit
    inline constexpr Eval THREE_PIECES_FREEDOM {36};

    Eval evaluate_position(SearchCtx& ctx, const Parameters& parameters, Eval evaluation_game_over,
        unsigned int plies_from_root, unsigned int& positions_evaluated);
    Eval calculate_material_both(SearchCtx& ctx);
    Eval calculate_freedom_both(SearchCtx& ctx);
    void get_players_freedom(SearchCtx& ctx, unsigned int& white, unsigned int& black);
    unsigned int get_piece_freedom(SearchCtx& ctx, Idx index);
}
