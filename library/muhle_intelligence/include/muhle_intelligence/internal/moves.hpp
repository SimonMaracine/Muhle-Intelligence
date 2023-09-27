#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>

#include <muhle_intelligence/definitions.hpp>

#include "muhle_intelligence/internal/array.hpp"

namespace muhle {
    struct SearchCtx;

    // Some thinking concludes that there cannot be more than 83 moves in a ply
    inline constexpr std::size_t MAX_MOVES = 83;

    void make_move(SearchCtx& ctx, const Move& move, Piece piece);
    void unmake_move(SearchCtx& ctx, const Move& move, Piece piece);
    void generate_moves(SearchCtx& ctx, Piece piece, Array<Move, MAX_MOVES>& moves);
    Move random_move(SearchCtx& ctx, Piece piece);

    namespace repetition {
        enum : std::uint64_t {
            None = 0b00,
            White = 0b01,
            Black = 0b10
        };

        struct Position {
            std::uint64_t part1 = 0;
            std::uint16_t part2 = 0;

            bool operator==(const Position& other) const {
                return part1 == other.part1 && part2 == other.part2;
            }
        };

        struct Node {
            Position position;
            const Node* previous = nullptr;
        };

        bool check_current_node(const Board& board, Player turn, Node& current, const Node* previous);
        Position make_position_bitboard(const Board& board, Player turn);
    }
}
