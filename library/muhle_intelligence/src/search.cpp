#include <unordered_map>
#include <string>
#include <limits>
#include <chrono>
#include <cassert>

#include <muhle_intelligence/definitions.hpp>

#include "muhle_intelligence/internal/search.hpp"
#include "muhle_intelligence/internal/array.hpp"
#include "muhle_intelligence/internal/moves.hpp"
#include "muhle_intelligence/internal/evaluation.hpp"
#include "muhle_intelligence/internal/various.hpp"

/*
    Maximizing player is white.
    Minimizing player is black.

    Evaluation is a signed integer.
    Value of 0 means equal position, positive means white is better, negative means black is better.
*/

namespace muhle {
    static constexpr Eval MIN_EVALUATION = std::numeric_limits<Eval>::min();
    static constexpr Eval MAX_EVALUATION = std::numeric_limits<Eval>::max();

    void Search::setup(std::unordered_map<std::string, int>& parameters) {
        // Here the default values are set
        parameters.try_emplace("PIECE", 7);
        parameters.try_emplace("FREEDOM", 1);
        parameters.try_emplace("END_GAME", MAX_EVALUATION);
        parameters.try_emplace("DEPTH", 5);

        this->parameters.PIECE = parameters.at("PIECE");
        this->parameters.FREEDOM = parameters.at("FREEDOM");
        this->parameters.END_GAME = parameters.at("END_GAME");
        this->parameters.DEPTH = parameters.at("DEPTH");
    }

    void Search::search(const Position& position, Result& result) {
        figure_out_position(position);

        Eval evaluation;

        const auto start = std::chrono::high_resolution_clock::now();

        if (position.player == Player::White) {
            evaluation = minimax_w(
                static_cast<unsigned int>(parameters.DEPTH),
                0u,
                MIN_EVALUATION,
                MAX_EVALUATION
            );
        } else {
            evaluation = minimax_b(
                static_cast<unsigned int>(parameters.DEPTH),
                0u,
                MIN_EVALUATION,
                MAX_EVALUATION
            );
        }

        const auto end = std::chrono::high_resolution_clock::now();

        result.result = best_move.move;
        result.player = position.player;
        result.time = std::chrono::duration<double>(end - start).count();
        result.evaluation = evaluation;
        result.positions_evaluated = positions_evaluated;

        result.done = true;
    }

    void Search::figure_out_position(const Position& position) {
        ctx.position = position.pieces;

        for (IterIdx i = 0; i < NODES; i++) {
            switch (ctx.position[i]) {
                case Piece::White:
                    ctx.white_pieces_on_board++;
                    break;
                case Piece::Black:
                    ctx.black_pieces_on_board++;
                    break;
                default:
                    break;
            }
        }

        ctx.plies = position.plies;
    }

    Eval Search::minimax_w(unsigned int depth, unsigned int plies_from_root, Eval alpha, Eval beta) {
        if (Eval evaluation_game_over = 0; depth == 0 || is_game_over(ctx, evaluation_game_over)) {
            return evaluate_position(ctx, parameters, evaluation_game_over, plies_from_root, positions_evaluated);
        }

        Eval max_evaluation = MIN_EVALUATION;

        Array<Move, MAX_MOVES> moves;
        generate_moves(ctx, Piece::White, moves);

        assert(moves.size() > 0);

        for (const Move& move : moves) {
            make_move(ctx, move, Piece::White);
            const Eval evaluation = minimax_b(depth - 1, plies_from_root + 1, alpha, beta);
            unmake_move(ctx, move, Piece::White);

            if (evaluation > max_evaluation) {
                max_evaluation = evaluation;

                if (plies_from_root == 0) {
                    best_move.move = move;
                    best_move.piece = Piece::White;
                }
            }

            // if (evaluation > alpha) {
            //     alpha = evaluation;  // FIXME
            // }

            // if (beta <= alpha) {
            //     break;
            // }
        }

        return max_evaluation;
    }

    Eval Search::minimax_b(unsigned int depth, unsigned int plies_from_root, Eval alpha, Eval beta) {
        if (Eval evaluation_game_over = 0; depth == 0 || is_game_over(ctx, evaluation_game_over)) {
            return evaluate_position(ctx, parameters, evaluation_game_over, plies_from_root, positions_evaluated);
        }

        Eval min_evaluation = MAX_EVALUATION;

        Array<Move, MAX_MOVES> moves;
        generate_moves(ctx, Piece::Black, moves);

        assert(moves.size() > 0);

        for (const Move& move : moves) {
            make_move(ctx, move, Piece::Black);
            const Eval evaluation = minimax_w(depth - 1, plies_from_root + 1, alpha, beta);
            unmake_move(ctx, move, Piece::Black);

            if (evaluation < min_evaluation) {
                min_evaluation = evaluation;

                if (plies_from_root == 0) {
                    best_move.move = move;
                    best_move.piece = Piece::Black;
                }
            }

            // if (evaluation < beta) {
            //     alpha = evaluation;  // FIXME
            // }

            // if (beta <= alpha) {
            //     break;
            // }
        }

        return min_evaluation;
    }
}
