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
        SearchNode& current_node {setup_nodes(position, prev_positions, prev_moves)};

        const auto start {std::chrono::high_resolution_clock::now()};

        const Eval evaluation {
            minimax(
                position.position.player,
                static_cast<unsigned int>(parameters.DEPTH),
                0u,
                MIN_EVALUATION,
                MAX_EVALUATION,
                current_node
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

    SearchNode& Search::setup_nodes(
        const SmnPosition& position,
        const std::vector<SmnPosition>& prev_positions,
        const std::vector<Move>& prev_moves
    ) {
        assert(!prev_positions.empty());
        assert(prev_positions.size() == prev_moves.size());

        auto iter1 = prev_positions.cbegin();
        auto iter2 = prev_moves.cbegin();

        while (iter1 != prev_positions.cend() && iter2 != prev_moves.cend()) {
            const SmnPosition& position {*iter1};
            const Move& move {*iter2};

            if (is_take_move(move)) {
                // Simply delete all currently created nodes
                nodes.clear();
            } else {
                SearchNode node;
                node.board = position.position.board;
                node.plies = position.plies;
                node.plies_without_mills = position.plies_without_mills;
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
        current_node.plies_without_mills = position.plies_without_mills;
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
    }

    Eval Search::minimax(
        Player player,
        unsigned int depth,
        unsigned int plies_from_root,
        Eval alpha,
        Eval beta,
        SearchNode& current_node
    ) {
        if (Eval game_over {0}; depth == 0 || is_game_over(current_node, game_over)) {
            return evaluate_position(current_node, parameters, game_over, plies_from_root, positions_evaluated);
        }

        if (repetition::check_repetition(current_node)) {
            return 0;
        }

        if (check_fifty_move(current_node)) {
            return 0;
        }

        if (player == Player::White) {
            Eval max_evaluation {MIN_EVALUATION};

            Moves moves;
            generate_moves(current_node, Piece::White, moves);

            assert(moves.size() > 0);

            for (const Move& move : moves) {
                SearchNode new_node;
                fill_node(new_node, current_node);

                play_move(new_node, move, Piece::White);

                const Eval evaluation {
                    minimax(Player::Black, depth - 1, plies_from_root + 1, alpha, beta, new_node)
                };

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

            Moves moves;
            generate_moves(current_node, Piece::Black, moves);

            assert(moves.size() > 0);

            for (const Move& move : moves) {
                SearchNode new_node;
                fill_node(new_node, current_node);

                play_move(new_node, move, Piece::Black);

                const Eval evaluation {
                    minimax(Player::White, depth - 1, plies_from_root + 1, alpha, beta, new_node)
                };

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
