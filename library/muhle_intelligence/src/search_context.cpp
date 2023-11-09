#include <cstdint>
#include <cassert>

#include <muhle_intelligence/definitions.hpp>

#include "muhle_intelligence/internal/moves.hpp"
#include "muhle_intelligence/internal/search_context.hpp"

namespace muhle {
    namespace repetition {
        bool check_repetition(const SearchNode& current) {
            unsigned int repetition {1};
            const SearchNode* node {current.previous};

            while (node != nullptr) {
                if (node->rep_position == current.rep_position) {
                    repetition++;

                    if (repetition == 3) {
                        return true;  // Early out
                    }
                }

                node = node->previous;
            }

            return false;
        }

        Position make_position_bitboard(const Board& board, Player player) {
            Position position;

            // First 48 bits (24 * 2) are for the board
            for (IterIdx i {0}; i < NODES; i++) {
                switch (board[i]) {
                    case Piece::None:
                        // Nothing
                        break;
                    case Piece::White:
                        position.bitboard |= White << i * 2;
                        break;
                    case Piece::Black:
                        position.bitboard |= Black << i * 2;
                        break;
                }
            }

            // Last bit means the player
            position.bitboard |= static_cast<std::uint64_t>(player) << 48;

            return position;
        }

    }

    void fill_node(SearchNode& destination, const SearchNode& source) {
        destination.board = source.board;
        destination.plies = source.plies;
        destination.plies_without_mills = source.plies_without_mills;
        destination.white_pieces_on_board = source.white_pieces_on_board;
        destination.black_pieces_on_board = source.black_pieces_on_board;

        // No need to copy rep_position

        destination.previous = &source;
    }

    bool check_fifty_move(const SearchNode& current) {
        assert(current.plies_without_mills <= 50);

        return current.plies_without_mills == 50;
    }
}
