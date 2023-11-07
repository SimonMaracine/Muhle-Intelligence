#include <vector>

#include <muhle_intelligence/definitions.hpp>

#include "muhle_intelligence/internal/moves.hpp"
#include "muhle_intelligence/internal/search_context.hpp"

namespace muhle {
    namespace repetition {
        bool check_node(const Board& board, Player turn, Node& current, const Node* previous) {
            const Position current_position {make_position_bitboard(board, turn)};

            current.previous = previous;
            current.position = current_position;

            const Node* node {previous};
            unsigned int repetition {1};

            while (node != nullptr) {
                if (node->position == current_position) {
                    repetition++;

                    if (repetition == 3) {
                        return true;  // Early out
                    }
                }

                node = node->previous;
            }

            return false;
        }

        Position make_position_bitboard(const Board& board, Player turn) {
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
            position.bitboard |= static_cast<std::uint64_t>(turn) << 48;

            return position;
        }
    }
}
