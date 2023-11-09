#pragma once

#include <cstdint>

#include <muhle_intelligence/definitions.hpp>

#include "muhle_intelligence/internal/moves.hpp"

namespace muhle {
    struct SearchNode;

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

        bool check_repetition(const SearchNode& current);
        Position make_position_bitboard(const Board& board, Player player);
    }

    struct SearchNode {
        Board board {};
        unsigned int plies {0};
        unsigned int plies_without_mills {0};
        repetition::Position rep_position;

        // Cache
        unsigned int white_pieces_on_board {0};
        unsigned int black_pieces_on_board {0};

        const SearchNode* previous {nullptr};
    };

    void fill_node(SearchNode& current, const SearchNode& previous);
    bool check_fifty_move(const SearchNode& current);

    struct Parameters {
        int PIECE {};
        int FREEDOM {};
        int END_GAME {};
        int DEPTH {};
    };
}
