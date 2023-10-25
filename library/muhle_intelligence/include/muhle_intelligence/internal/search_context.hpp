#pragma once

#include <vector>

#include <muhle_intelligence/definitions.hpp>

#include "muhle_intelligence/internal/moves.hpp"

namespace muhle {
    struct SearchCtx {
        Board board {};
        unsigned int white_pieces_on_board {};
        unsigned int black_pieces_on_board {};
        unsigned int plies {};

        struct {
            const repetition::Node* previous = nullptr;
            std::vector<repetition::Node> nodes;
        } previous;
    };

    struct Parameters {
        int PIECE {};
        int FREEDOM {};
        int END_GAME {};
        int DEPTH {};
    };
}
