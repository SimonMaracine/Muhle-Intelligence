#include <cassert>

#include <muhle_intelligence/definitions.hpp>

#include "muhle_intelligence/internal/search_context.hpp"
#include "muhle_intelligence/internal/evaluation.hpp"

namespace muhle {
    Eval evaluate_position(SearchCtx& ctx, const Parameters& parameters, Eval evaluation_game_over,
            unsigned int plies_from_root, unsigned int& positions_evaluated) {
        positions_evaluated++;

        Eval evaluation = 0;

        if (evaluation_game_over != 0) {
            // Teach what is end game
            evaluation = evaluation_game_over * static_cast<Eval>(parameters.END_GAME);

            // Use the quickest solution
            evaluation += -evaluation_game_over * plies_from_root;

            return evaluation;
        }

        const Eval evaluation_material = calculate_material_both(ctx);

        // Calculate number of pieces
        evaluation += evaluation_material * static_cast<Eval>(parameters.PIECE);

        const Eval evaluation_freedom = calculate_freedom_both(ctx);

        // Calculate pieces' freedom
        evaluation += evaluation_freedom * static_cast<Eval>(parameters.FREEDOM);

        return evaluation;
    }

    Eval calculate_material_both(SearchCtx& ctx) {
        Eval evaluation_material = 0;

        for (IterIdx i = 0; i < NODES; i++) {
            evaluation_material += static_cast<Eval>(ctx.position[i]);
        }

        return evaluation_material;
    }

    Eval calculate_freedom_both(SearchCtx& ctx) {
        Eval evaluation_freedom = 0;

        const bool phase_two = ctx.plies >= 18;
        const bool three_piece_white = ctx.white_pieces_on_board == 3;
        const bool three_piece_black = ctx.black_pieces_on_board == 3;

        if (three_piece_white && three_piece_black && phase_two) {  // Both have three pieces
            evaluation_freedom = 0;
        } else if (three_piece_white && phase_two) {  // Only white has three pieces
            evaluation_freedom += THREE_PIECES_FREEDOM;

            for (IterIdx i = 0; i < NODES; i++) {
                if (ctx.position[i] != Piece::Black) {
                    continue;
                }

                evaluation_freedom += (
                    static_cast<Eval>(get_piece_freedom(ctx, i)) * static_cast<Eval>(ctx.position[i])
                );
            }
        } else if (three_piece_black && phase_two) {  // Only black has three pieces
            evaluation_freedom -= THREE_PIECES_FREEDOM;

            for (IterIdx i = 0; i < NODES; i++) {
                if (ctx.position[i] != Piece::White) {
                    continue;
                }

                evaluation_freedom += (
                    static_cast<Eval>(get_piece_freedom(ctx, i)) * static_cast<Eval>(ctx.position[i])
                );
            }
        } else {  // No one has three pieces
            for (IterIdx i = 0; i < NODES; i++) {
                if (ctx.position[i] == Piece::None) {
                    continue;
                }

                evaluation_freedom += (
                    static_cast<Eval>(get_piece_freedom(ctx, i)) * static_cast<Eval>(ctx.position[i])
                );
            }
        }

        return evaluation_freedom;
    }

    unsigned int get_material(SearchCtx& ctx, Piece piece) {
        assert(piece != Piece::None);

        unsigned int piece_count = 0;

        for (IterIdx i = 0; i < NODES; i++) {
            piece_count += ctx.position[i] == piece;
        }

        return piece_count;
    }

    void get_players_freedom(SearchCtx& ctx, unsigned int& white, unsigned int& black) {
        unsigned int white_free_positions = 0;
        unsigned int black_free_positions = 0;

        const bool phase_two = ctx.plies >= 18;
        const bool three_piece_white = ctx.white_pieces_on_board == 3;
        const bool three_piece_black = ctx.black_pieces_on_board == 3;

        if (three_piece_white && three_piece_black && phase_two) {  // Both have three pieces
            white_free_positions = THREE_PIECES_FREEDOM;
            black_free_positions = THREE_PIECES_FREEDOM;
        } else if (three_piece_white && phase_two) {  // White has three pieces
            white_free_positions = THREE_PIECES_FREEDOM;

            for (IterIdx i = 0; i < NODES; i++) {
                if (ctx.position[i] != Piece::Black) {
                    continue;
                }

                black_free_positions += get_piece_freedom(ctx, i);
            }
        } else if (three_piece_black && phase_two) {  // Black has three pieces
            black_free_positions = THREE_PIECES_FREEDOM;

            for (IterIdx i = 0; i < NODES; i++) {
                if (ctx.position[i] != Piece::White) {
                    continue;
                }

                white_free_positions += get_piece_freedom(ctx, i);
            }
        } else {  // No one has three pieces
            for (IterIdx i = 0; i < NODES; i++) {
                switch (ctx.position[i]) {
                    case Piece::White:
                        white_free_positions += get_piece_freedom(ctx, i);
                        break;
                    case Piece::Black:
                        black_free_positions += get_piece_freedom(ctx, i);
                        break;
                    default:
                        break;
                }
            }
        }

        white = white_free_positions;
        black = black_free_positions;
    }

    unsigned int get_piece_freedom(SearchCtx& ctx, Idx index) {
        assert(ctx.position[index] != Piece::None);

        unsigned int freedom = 0;

        switch (index) {
            case 0:
                freedom += ctx.position[1] == Piece::None;
                freedom += ctx.position[9] == Piece::None;
                break;
            case 1:
                freedom += ctx.position[0] == Piece::None;
                freedom += ctx.position[2] == Piece::None;
                freedom += ctx.position[4] == Piece::None;
                break;
            case 2:
                freedom += ctx.position[1] == Piece::None;
                freedom += ctx.position[14] == Piece::None;
                break;
            case 3:
                freedom += ctx.position[4] == Piece::None;
                freedom += ctx.position[10] == Piece::None;
                break;
            case 4:
                freedom += ctx.position[1] == Piece::None;
                freedom += ctx.position[3] == Piece::None;
                freedom += ctx.position[5] == Piece::None;
                freedom += ctx.position[7] == Piece::None;
                break;
            case 5:
                freedom += ctx.position[4] == Piece::None;
                freedom += ctx.position[13] == Piece::None;
                break;
            case 6:
                freedom += ctx.position[7] == Piece::None;
                freedom += ctx.position[11] == Piece::None;
                break;
            case 7:
                freedom += ctx.position[4] == Piece::None;
                freedom += ctx.position[6] == Piece::None;
                freedom += ctx.position[8] == Piece::None;
                break;
            case 8:
                freedom += ctx.position[7] == Piece::None;
                freedom += ctx.position[12] == Piece::None;
                break;
            case 9:
                freedom += ctx.position[0] == Piece::None;
                freedom += ctx.position[10] == Piece::None;
                freedom += ctx.position[21] == Piece::None;
                break;
            case 10:
                freedom += ctx.position[3] == Piece::None;
                freedom += ctx.position[9] == Piece::None;
                freedom += ctx.position[11] == Piece::None;
                freedom += ctx.position[18] == Piece::None;
                break;
            case 11:
                freedom += ctx.position[6] == Piece::None;
                freedom += ctx.position[10] == Piece::None;
                freedom += ctx.position[15] == Piece::None;
                break;
            case 12:
                freedom += ctx.position[8] == Piece::None;
                freedom += ctx.position[13] == Piece::None;
                freedom += ctx.position[17] == Piece::None;
                break;
            case 13:
                freedom += ctx.position[5] == Piece::None;
                freedom += ctx.position[12] == Piece::None;
                freedom += ctx.position[14] == Piece::None;
                freedom += ctx.position[20] == Piece::None;
                break;
            case 14:
                freedom += ctx.position[2] == Piece::None;
                freedom += ctx.position[13] == Piece::None;
                freedom += ctx.position[23] == Piece::None;
                break;
            case 15:
                freedom += ctx.position[11] == Piece::None;
                freedom += ctx.position[16] == Piece::None;
                break;
            case 16:
                freedom += ctx.position[15] == Piece::None;
                freedom += ctx.position[17] == Piece::None;
                freedom += ctx.position[19] == Piece::None;
                break;
            case 17:
                freedom += ctx.position[12] == Piece::None;
                freedom += ctx.position[16] == Piece::None;
                break;
            case 18:
                freedom += ctx.position[10] == Piece::None;
                freedom += ctx.position[19] == Piece::None;
                break;
            case 19:
                freedom += ctx.position[16] == Piece::None;
                freedom += ctx.position[18] == Piece::None;
                freedom += ctx.position[20] == Piece::None;
                freedom += ctx.position[22] == Piece::None;
                break;
            case 20:
                freedom += ctx.position[13] == Piece::None;
                freedom += ctx.position[19] == Piece::None;
                break;
            case 21:
                freedom += ctx.position[9] == Piece::None;
                freedom += ctx.position[22] == Piece::None;
                break;
            case 22:
                freedom += ctx.position[19] == Piece::None;
                freedom += ctx.position[21] == Piece::None;
                freedom += ctx.position[23] == Piece::None;
                break;
            case 23:
                freedom += ctx.position[14] == Piece::None;
                freedom += ctx.position[22] == Piece::None;
                break;
        }

        return freedom;
    }
}
