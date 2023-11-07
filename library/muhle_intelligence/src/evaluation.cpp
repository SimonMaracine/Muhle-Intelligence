#include <cassert>

#include <muhle_intelligence/definitions.hpp>

#include "muhle_intelligence/internal/search_context.hpp"
#include "muhle_intelligence/internal/evaluation.hpp"

namespace muhle {
    Eval evaluate_position(
        const SearchNode& node,
        const Parameters& parameters,
        Eval evaluation_game_over,
        unsigned int plies_from_root,
        unsigned int& positions_evaluated
    ) {
        positions_evaluated++;

        Eval evaluation {0};

        if (evaluation_game_over != 0) {
            // Teach what is end game
            evaluation = evaluation_game_over * static_cast<Eval>(parameters.END_GAME);

            // Use the quickest solution
            evaluation += -evaluation_game_over * plies_from_root;

            return evaluation;
        }

        const Eval evaluation_material {calculate_material_both(node)};

        // Calculate number of pieces
        evaluation += evaluation_material * static_cast<Eval>(parameters.PIECE);

        const Eval evaluation_freedom {calculate_freedom_both(node)};

        // Calculate pieces' freedom
        evaluation += evaluation_freedom * static_cast<Eval>(parameters.FREEDOM);

        return evaluation;
    }

    Eval calculate_material_both(const SearchNode& node) {
        return static_cast<Eval>(node.white_pieces_on_board - node.black_pieces_on_board);
    }

    Eval calculate_freedom_both(const SearchNode& node) {
        Eval evaluation_freedom {0};

        const bool phase_two {node.plies >= 18};
        const bool three_piece_white {node.white_pieces_on_board == 3};
        const bool three_piece_black {node.black_pieces_on_board == 3};

        if (three_piece_white && three_piece_black && phase_two) {  // Both have three pieces
            evaluation_freedom = 0;
        } else if (three_piece_white && phase_two) {  // Only white has three pieces
            evaluation_freedom += THREE_PIECES_FREEDOM;

            for (IterIdx i {0}; i < NODES; i++) {
                if (node.board[i] != Piece::Black) {
                    continue;
                }

                evaluation_freedom += (
                    static_cast<Eval>(get_piece_freedom(node, i)) * static_cast<Eval>(node.board[i])
                );
            }
        } else if (three_piece_black && phase_two) {  // Only black has three pieces
            evaluation_freedom -= THREE_PIECES_FREEDOM;

            for (IterIdx i {0}; i < NODES; i++) {
                if (node.board[i] != Piece::White) {
                    continue;
                }

                evaluation_freedom += (
                    static_cast<Eval>(get_piece_freedom(node, i)) * static_cast<Eval>(node.board[i])
                );
            }
        } else {  // No one has three pieces
            for (IterIdx i {0}; i < NODES; i++) {
                if (node.board[i] == Piece::None) {
                    continue;
                }

                evaluation_freedom += (
                    static_cast<Eval>(get_piece_freedom(node, i)) * static_cast<Eval>(node.board[i])
                );
            }
        }

        return evaluation_freedom;
    }

    void get_players_freedom(const SearchNode& node, unsigned int& white, unsigned int& black) {
        unsigned int white_free_positions {0};
        unsigned int black_free_positions {0};

        const bool phase_two {node.plies >= 18};
        const bool three_piece_white {node.white_pieces_on_board == 3};
        const bool three_piece_black {node.black_pieces_on_board == 3};

        if (three_piece_white && three_piece_black && phase_two) {  // Both have three pieces
            white_free_positions = THREE_PIECES_FREEDOM;
            black_free_positions = THREE_PIECES_FREEDOM;
        } else if (three_piece_white && phase_two) {  // White has three pieces
            white_free_positions = THREE_PIECES_FREEDOM;

            for (IterIdx i {0}; i < NODES; i++) {
                if (node.board[i] != Piece::Black) {
                    continue;
                }

                black_free_positions += get_piece_freedom(node, i);
            }
        } else if (three_piece_black && phase_two) {  // Black has three pieces
            black_free_positions = THREE_PIECES_FREEDOM;

            for (IterIdx i {0}; i < NODES; i++) {
                if (node.board[i] != Piece::White) {
                    continue;
                }

                white_free_positions += get_piece_freedom(node, i);
            }
        } else {  // No one has three pieces
            for (IterIdx i {0}; i < NODES; i++) {
                switch (node.board[i]) {
                    case Piece::White:
                        white_free_positions += get_piece_freedom(node, i);
                        break;
                    case Piece::Black:
                        black_free_positions += get_piece_freedom(node, i);
                        break;
                    default:
                        break;
                }
            }
        }

        white = white_free_positions;
        black = black_free_positions;
    }

    unsigned int get_piece_freedom(const SearchNode& node, Idx index) {
        assert(node.board[index] != Piece::None);

        unsigned int freedom {0};

        switch (index) {
            case 0:
                freedom += node.board[1] == Piece::None;
                freedom += node.board[9] == Piece::None;
                break;
            case 1:
                freedom += node.board[0] == Piece::None;
                freedom += node.board[2] == Piece::None;
                freedom += node.board[4] == Piece::None;
                break;
            case 2:
                freedom += node.board[1] == Piece::None;
                freedom += node.board[14] == Piece::None;
                break;
            case 3:
                freedom += node.board[4] == Piece::None;
                freedom += node.board[10] == Piece::None;
                break;
            case 4:
                freedom += node.board[1] == Piece::None;
                freedom += node.board[3] == Piece::None;
                freedom += node.board[5] == Piece::None;
                freedom += node.board[7] == Piece::None;
                break;
            case 5:
                freedom += node.board[4] == Piece::None;
                freedom += node.board[13] == Piece::None;
                break;
            case 6:
                freedom += node.board[7] == Piece::None;
                freedom += node.board[11] == Piece::None;
                break;
            case 7:
                freedom += node.board[4] == Piece::None;
                freedom += node.board[6] == Piece::None;
                freedom += node.board[8] == Piece::None;
                break;
            case 8:
                freedom += node.board[7] == Piece::None;
                freedom += node.board[12] == Piece::None;
                break;
            case 9:
                freedom += node.board[0] == Piece::None;
                freedom += node.board[10] == Piece::None;
                freedom += node.board[21] == Piece::None;
                break;
            case 10:
                freedom += node.board[3] == Piece::None;
                freedom += node.board[9] == Piece::None;
                freedom += node.board[11] == Piece::None;
                freedom += node.board[18] == Piece::None;
                break;
            case 11:
                freedom += node.board[6] == Piece::None;
                freedom += node.board[10] == Piece::None;
                freedom += node.board[15] == Piece::None;
                break;
            case 12:
                freedom += node.board[8] == Piece::None;
                freedom += node.board[13] == Piece::None;
                freedom += node.board[17] == Piece::None;
                break;
            case 13:
                freedom += node.board[5] == Piece::None;
                freedom += node.board[12] == Piece::None;
                freedom += node.board[14] == Piece::None;
                freedom += node.board[20] == Piece::None;
                break;
            case 14:
                freedom += node.board[2] == Piece::None;
                freedom += node.board[13] == Piece::None;
                freedom += node.board[23] == Piece::None;
                break;
            case 15:
                freedom += node.board[11] == Piece::None;
                freedom += node.board[16] == Piece::None;
                break;
            case 16:
                freedom += node.board[15] == Piece::None;
                freedom += node.board[17] == Piece::None;
                freedom += node.board[19] == Piece::None;
                break;
            case 17:
                freedom += node.board[12] == Piece::None;
                freedom += node.board[16] == Piece::None;
                break;
            case 18:
                freedom += node.board[10] == Piece::None;
                freedom += node.board[19] == Piece::None;
                break;
            case 19:
                freedom += node.board[16] == Piece::None;
                freedom += node.board[18] == Piece::None;
                freedom += node.board[20] == Piece::None;
                freedom += node.board[22] == Piece::None;
                break;
            case 20:
                freedom += node.board[13] == Piece::None;
                freedom += node.board[19] == Piece::None;
                break;
            case 21:
                freedom += node.board[9] == Piece::None;
                freedom += node.board[22] == Piece::None;
                break;
            case 22:
                freedom += node.board[19] == Piece::None;
                freedom += node.board[21] == Piece::None;
                freedom += node.board[23] == Piece::None;
                break;
            case 23:
                freedom += node.board[14] == Piece::None;
                freedom += node.board[22] == Piece::None;
                break;
        }

        return freedom;
    }
}
