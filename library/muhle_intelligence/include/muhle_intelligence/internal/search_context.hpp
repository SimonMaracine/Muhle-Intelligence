#pragma once

#include <vector>

#include <muhle_intelligence/definitions.hpp>

#include "muhle_intelligence/internal/moves.hpp"

namespace muhle {
    namespace repetition {
        enum : std::uint64_t {
            None = 0b00,
            White = 0b01,
            Black = 0b10
        };

        struct Position {
            std::uint64_t bitboard {0};

            bool operator==(const Position& other) const {
                return bitboard == other.bitboard;
            }
        };

        struct Node {
            Position position;
            const Node* previous {nullptr};
        };

        // Node previous can be null
        bool check_node(const Board& board, Player turn, Node& current, const Node* previous);
        Position make_position_bitboard(const Board& board, Player turn);
    }

    struct SearchCtx {
        Board board {};
        unsigned int plies {0};
        unsigned int plies_without_mills {0};

        // Cache
        unsigned int white_pieces_on_board {};
        unsigned int black_pieces_on_board {};

        struct {
            const repetition::Node* previous {nullptr};
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
