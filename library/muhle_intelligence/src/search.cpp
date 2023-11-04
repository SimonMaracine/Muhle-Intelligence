#include <unordered_map>
#include <string>
#include <vector>
#include <limits>
#include <chrono>
#include <cassert>
#include <cstddef>

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
    static constexpr Eval MIN_EVALUATION {std::numeric_limits<Eval>::min()};
    static constexpr Eval MAX_EVALUATION {std::numeric_limits<Eval>::max()};

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

    Move Search::search(
        const SmnPosition& position,
        Iter prev_positions_begin,
        Iter prev_positions_end,
        const std::vector<Move>& prev_moves,
        Result& result
    ) {
        setup_position(position, prev_positions_begin, prev_positions_end, prev_moves);

        const auto start {std::chrono::high_resolution_clock::now()};

        const Eval evaluation {minimax(
            position.position.player,
            static_cast<unsigned int>(parameters.DEPTH),
            0u,
            MIN_EVALUATION,
            MAX_EVALUATION,
            ctx.previous.previous
        )};

        const auto end {std::chrono::high_resolution_clock::now()};

        result.result = best_move.move;
        result.player = position.position.player;
        result.time = std::chrono::duration<double>(end - start).count();
        result.evaluation = evaluation;
        result.positions_evaluated = positions_evaluated;

        result.done = true;

        return best_move.move;
    }

    void Search::setup_position(
        const SmnPosition& position,
        Iter prev_positions_begin,
        Iter prev_positions_end,
        const std::vector<Move>& prev_moves
    ) {
        ctx.board = position.position.board;
        ctx.plies = position.plies;

        for (IterIdx i {0}; i < NODES; i++) {
            switch (position.position.board[i]) {
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

        if (prev_positions_begin == prev_positions_end) {
            // Pointer ctx.previous.previous will remain null
            return;
        }

        auto iter1 = prev_positions_begin;
        auto iter2 = prev_moves.cbegin();

        while (iter1 != prev_positions_end && iter2 != prev_moves.cend()) {
            const Position& position {*iter1};
            const Move& move {*iter2};

            if (is_take_move(move)) {
                // Simply cut all previous nodes
                ctx.previous.nodes.clear();
            } else {
                repetition::Node node;
                node.position = repetition::make_position_bitboard(position.board, position.player);

                ctx.previous.nodes.push_back(node);
            }

            iter1++;
            iter2++;
        }

        // Setup pointers only now
        for (std::size_t i {ctx.previous.nodes.size()}; i > 0; i--) {
            const std::size_t I {i - 1};

            if (I > 0) {
                ctx.previous.nodes[I].previous = &ctx.previous.nodes[I - 1];
            }
        }

        if (!ctx.previous.nodes.empty()) {
            ctx.previous.previous = &ctx.previous.nodes[ctx.previous.nodes.size() - 1];
        }
    }

    Eval Search::minimax(
        Player player,
        unsigned int depth,
        unsigned int plies_from_root,
        Eval alpha,
        Eval beta,
        const repetition::Node* previous_node
    ) {
        if (Eval evaluation_game_over {0}; depth == 0 || is_game_over(ctx, evaluation_game_over)) {
            return evaluate_position(ctx, parameters, evaluation_game_over, plies_from_root, positions_evaluated);
        }

        repetition::Node current_node;

        if (repetition::check_current_node(ctx.board, player, current_node, previous_node)) {
            return 0;  // This means a tie
        }

        if (player == Player::White) {
            Eval max_evaluation {MIN_EVALUATION};

            Array<Move, MAX_MOVES> moves;
            generate_moves(ctx, Piece::White, moves);

            assert(moves.size() > 0);

            for (const Move& move : moves) {
                const repetition::Node* node {is_take_move(move) ? nullptr : &current_node};

                make_move(ctx, move, Piece::White);
                const Eval evaluation {minimax(Player::Black, depth - 1, plies_from_root + 1, alpha, beta, node)};
                unmake_move(ctx, move, Piece::White);

                if (evaluation > max_evaluation) {
                    max_evaluation = evaluation;

                    if (plies_from_root == 0) {
                        best_move.move = move;
                        best_move.piece = Piece::White;
                    }
                }
            }

            return max_evaluation;
        } else {
            Eval min_evaluation {MAX_EVALUATION};

            Array<Move, MAX_MOVES> moves;
            generate_moves(ctx, Piece::Black, moves);

            assert(moves.size() > 0);

            for (const Move& move : moves) {
                const repetition::Node* node {is_take_move(move) ? nullptr : &current_node};

                make_move(ctx, move, Piece::Black);
                const Eval evaluation {minimax(Player::White, depth - 1, plies_from_root + 1, alpha, beta, node)};
                unmake_move(ctx, move, Piece::Black);

                if (evaluation < min_evaluation) {
                    min_evaluation = evaluation;

                    if (plies_from_root == 0) {
                        best_move.move = move;
                        best_move.piece = Piece::Black;
                    }
                }
            }

            return min_evaluation;
        }
    }
}
