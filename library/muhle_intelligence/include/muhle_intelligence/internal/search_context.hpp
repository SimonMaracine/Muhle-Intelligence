#pragma once

#include <array>

#include <muhle_intelligence/definitions.hpp>

namespace muhle {
    struct SearchCtx {
        std::array<Piece, NODES> position {};
        unsigned int white_pieces_on_board {};
        unsigned int black_pieces_on_board {};
        unsigned int plies {};
    };

    struct Parameters {
        int PIECE {};
        int FREEDOM {};
        int END_GAME {};
        int DEPTH {};
    };
}
