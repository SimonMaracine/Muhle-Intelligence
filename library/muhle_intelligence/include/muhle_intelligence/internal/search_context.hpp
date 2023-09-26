#pragma once

#include <muhle_intelligence/definitions.hpp>

#include "muhle_intelligence/internal/moves.hpp"

namespace muhle {
    struct SearchCtx {
        Pieces position {};
        unsigned int white_pieces_on_board {};
        unsigned int black_pieces_on_board {};
        unsigned int plies {};
        ThreefoldRepetition repetition;
    };

    struct Parameters {
        int PIECE {};
        int FREEDOM {};
        int END_GAME {};
        int DEPTH {};
    };
}
