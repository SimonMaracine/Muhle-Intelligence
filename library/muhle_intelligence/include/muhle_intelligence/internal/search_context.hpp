#pragma once

#include <cstdint>
#include <optional>

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

        // Node previous can be null
        bool check_repetition(
            const Board& board,
            Player player,
            const SearchNode& current,
            const SearchNode* previous
        );

        Position make_position_bitboard(const Board& board, Player player);
    }

    void fill_node(SearchNode& current, const SearchNode* previous, Player player);

    struct SearchNode {
        Board board {};
        unsigned int plies {0};
        unsigned int plies_without_mills {0};
        std::optional<repetition::Position> rep_position;

        // Cache
        unsigned int white_pieces_on_board {};
        unsigned int black_pieces_on_board {};

        const SearchNode* previous {nullptr};
    };

    // struct SearchCtx {
    //     Board board {};
    //     unsigned int plies {0};
    //     unsigned int plies_without_mills {0};

    //     // Cache
    //     unsigned int white_pieces_on_board {};
    //     unsigned int black_pieces_on_board {};

    //     struct {
    //         const repetition::Node* previous {nullptr};
    //         std::vector<repetition::Node> nodes;  // FIXME this
    //     } previous;
    // };

    struct Parameters {
        int PIECE {};
        int FREEDOM {};
        int END_GAME {};
        int DEPTH {};
    };
}
