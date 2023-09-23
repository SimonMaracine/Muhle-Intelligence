#pragma once

#include <cstddef>
#include <forward_list>
#include <array>
#include <tuple>

#include <muhle_intelligence/definitions.hpp>

#include "muhle_intelligence/internal/search_context.hpp"
#include "muhle_intelligence/internal/array.hpp"

namespace muhle {
    // Some thinking concludes that there cannot be more than 58 moves in a ply
    inline constexpr std::size_t MAX_MOVES = 58;

    void make_move(SearchCtx& ctx, const Move& move, Piece piece);
    void unmake_move(SearchCtx& ctx, const Move& move, Piece piece);
    void generate_moves(SearchCtx& ctx, Piece piece, Array<Move, MAX_MOVES>& moves);
    Move random_move(SearchCtx& ctx, Piece piece);

    class ThreefoldRepetition {
    public:
        enum BinaryPiece {
            Black = 0b00,
            None = 0b01,
            White = 0b10
        };

        struct Position {
            std::uint64_t part1 = 0;
            std::uint16_t part2 = 0;

            bool operator==(const Position& other) const {
                return part1 == other.part1 && part2 == other.part2;
            }
        };

        bool threefold_repetition();
        void clear_repetition();
    private:
        Position write_position(const std::array<Piece, NODES>& pieces, Player turn);
        std::pair<std::array<Piece, NODES>, Player> read_position(Position position);

        std::forward_list<Position> ones;
        std::forward_list<Position> twos;
    };
}
