#include <unordered_map>
#include <string>
#include <vector>
#include <limits>
#include <chrono>
#include <cstddef>
#include <algorithm>
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
        const std::vector<SmnPosition>& prev_positions,
        const std::vector<Move>& prev_moves,
        Result& result
    ) {
        const SearchNode* previous_node {setup_nodes(position, prev_positions, prev_moves)};

        const auto start {std::chrono::high_resolution_clock::now()};

        const Eval evaluation {
            minimax(
                position.position.player,
                static_cast<unsigned int>(parameters.DEPTH),
                0u,
                MIN_EVALUATION,
                MAX_EVALUATION,
                previous_node
            )
        };

        const auto end {std::chrono::high_resolution_clock::now()};

        result.result = best_move.move;
        result.player = position.position.player;
        result.time = std::chrono::duration<double>(end - start).count();
        result.evaluation = evaluation;
        result.positions_evaluated = positions_evaluated;

        result.done = true;

        return best_move.move;
    }

    const SearchNode* Search::setup_nodes(
        const SmnPosition& position,
        const std::vector<SmnPosition>& prev_positions,
        const std::vector<Move>& prev_moves
    ) {
        assert(!prev_positions.empty());
        assert(prev_positions.size() - 1 == prev_moves.size());

        auto iter1 = prev_positions.cbegin();
        auto iter2 = prev_moves.cbegin();

        while (iter1 != prev_positions.cend() && iter2 != prev_moves.cend()) {
            const SmnPosition& position {*iter1};
            const Move& move {*iter2};

            if (is_take_move(move)) {
                // Simply cut all previous nodes
                // ctx.previous.nodes.clear();
            } else {
                SearchNode node;
                node.board = position.position.board;
                node.plies = position.plies;
                node.plies_without_mills = position.plies_witout_mills;
                node.rep_position = (
                    repetition::make_position_bitboard(
                        position.position.board,
                        position.position.player
                    )
                );
                count_pieces(
                    position.position.board,
                    node.white_pieces_on_board,
                    node.black_pieces_on_board
                );

                nodes.push_back(node);
            }

            iter1++;
            iter2++;
        }

        SearchNode current_node;
        current_node.board = position.position.board;
        current_node.plies = position.plies;
        current_node.plies_without_mills = position.plies_witout_mills;
        current_node.rep_position = (
            repetition::make_position_bitboard(
                position.position.board,
                position.position.player
            )
        );
        count_pieces(
            position.position.board,
            current_node.white_pieces_on_board,
            current_node.black_pieces_on_board
        );

        nodes.push_back(current_node);

        // Setup pointers only now
        for (std::size_t i {nodes.size()}; i > 1; i--) {
            const std::size_t I {i - 1};

            nodes[I].previous = &nodes[I - 1];
        }

        // Array nodes cannot be empty
        // It contains the current position and potentially previous positions
        assert(!nodes.empty());

        return nodes.back();

        // ctx.board = position.position.board;
        // ctx.plies = position.plies;

        // for (IterIdx i {0}; i < NODES; i++) {
        //     switch (position.position.board[i]) {
        //         case Piece::White:
        //             ctx.white_pieces_on_board++;
        //             break;
        //         case Piece::Black:
        //             ctx.black_pieces_on_board++;
        //             break;
        //         default:
        //             break;
        //     }
        // }

        // if (prev_positions_begin == prev_positions_end) {
        //     // Pointer ctx.previous.previous will remain null
        //     return;
        // }

        // auto iter1 = prev_positions_begin;
        // auto iter2 = prev_moves.cbegin();

        // while (iter1 != prev_positions_end && iter2 != prev_moves.cend()) {
        //     const Position& position {*iter1};
        //     const Move& move {*iter2};

        //     if (is_take_move(move)) {
        //         // Simply cut all previous nodes
        //         ctx.previous.nodes.clear();
        //     } else {
        //         SearchNode node;
        //         node.rep_position = repetition::make_position_bitboard(position.board, position.player);

        //         ctx.previous.nodes.push_back(node);
        //     }

        //     iter1++;
        //     iter2++;
        // }

        // // Setup pointers only now
        // for (std::size_t i {ctx.previous.nodes.size()}; i > 0; i--) {
        //     const std::size_t I {i - 1};

        //     if (I > 0) {
        //         ctx.previous.nodes[I].previous = &ctx.previous.nodes[I - 1];
        //     }
        // }

        // if (!ctx.previous.nodes.empty()) {
        //     ctx.previous.previous = &ctx.previous.nodes[ctx.previous.nodes.size() - 1];
        // }
    }

    Eval Search::minimax(
        Player player,
        unsigned int depth,
        unsigned int plies_from_root,
        Eval alpha,
        Eval beta,
        const SearchNode& previous_node
    ) {
        SearchNode new_node;
        fill_node(new_node, previous_node, player);

        if (Eval game_over {0}; depth == 0 || is_game_over(new_node, game_over)) {
            return evaluate_position(new_node, parameters, game_over, plies_from_root, positions_evaluated);
        }

        if (repetition::check_repetition(new_node.board, player, new_node, previous_node)) {
            return 0;
        }

        // TODO 50-move rule

        if (player == Player::White) {
            Eval max_evaluation {MIN_EVALUATION};

            Array<Move, MAX_MOVES> moves;
            generate_moves(current_node, Piece::White, moves);

            assert(moves.size() > 0);

            for (const Move& move : moves) {
                // const SearchNode* node {is_take_move(move) ? nullptr : &current_node};

                play_move(current_node, move, Piece::White);
                const Eval evaluation {
                    minimax(Player::Black, depth - 1, plies_from_root + 1, alpha, beta, &new_node)
                };
                // unmake_move(node, move, Piece::White);

                if (evaluation > max_evaluation) {
                    max_evaluation = evaluation;

                    if (plies_from_root == 0) {
                        best_move.move = move;
                        best_move.piece = Piece::White;
                    }
                }

                alpha = std::max(alpha, evaluation);

                if (beta <= alpha) {
                    break;
                }
            }

            return max_evaluation;
        } else {
            Eval min_evaluation {MAX_EVALUATION};

            Array<Move, MAX_MOVES> moves;
            generate_moves(new_node, Piece::Black, moves);

            assert(moves.size() > 0);

            for (const Move& move : moves) {
                // const SearchNode* node {is_take_move(move) ? nullptr : &current_node};

                play_move(new_node, move, Piece::Black);
                const Eval evaluation {
                    minimax(Player::White, depth - 1, plies_from_root + 1, alpha, beta, &new_node)
                };
                // unmake_move(node, move, Piece::Black);

                if (evaluation < min_evaluation) {
                    min_evaluation = evaluation;

                    if (plies_from_root == 0) {
                        best_move.move = move;
                        best_move.piece = Piece::Black;
                    }
                }

                beta = std::min(beta, evaluation);

                if (beta <= alpha) {
                    break;
                }
            }

            return min_evaluation;
        }
    }
}
