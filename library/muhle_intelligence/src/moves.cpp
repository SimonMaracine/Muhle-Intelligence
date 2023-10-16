#include <cstddef>
#include <cstdint>
#include <utility>
#include <cassert>
#include <random>

#include <muhle_intelligence/definitions.hpp>

#include "muhle_intelligence/internal/search_context.hpp"
#include "muhle_intelligence/internal/array.hpp"
#include "muhle_intelligence/internal/moves.hpp"
#include "muhle_intelligence/internal/various.hpp"

namespace muhle {
    static std::mt19937 random = std::mt19937(std::random_device()());

    static void make_place_move(SearchCtx& ctx, Piece piece, Idx node_index) {
        ctx.board[node_index] = piece;
    }

    static void unmake_place_move(SearchCtx& ctx, Idx node_index) {
        ctx.board[node_index] = Piece::None;
    }

    static void make_move_move(SearchCtx& ctx, Piece piece, Idx node_source_index, Idx node_destination_index) {
        ctx.board[node_source_index] = Piece::None;
        ctx.board[node_destination_index] = piece;
    }

    static void unmake_move_move(SearchCtx& ctx, Piece piece, Idx node_source_index, Idx node_destination_index) {
        ctx.board[node_source_index] = piece;
        ctx.board[node_destination_index] = Piece::None;
    }

    static Move create_place(Idx node_index) {
        Move move;
        move.place.node_index = node_index;
        move.type = MoveType::Place;

        return move;
    }

    static Move create_move(Idx node_source_index, Idx node_destination_index) {
        Move move;
        move.move.node_source_index = node_source_index;
        move.move.node_destination_index = node_destination_index;
        move.type = MoveType::Move;

        return move;
    }

    static Move create_place_take(Idx node_index, Idx node_take_index) {
        Move move;
        move.place_take.node_index = node_index;
        move.place_take.node_take_index = node_take_index;
        move.type = MoveType::PlaceTake;

        return move;
    }

    static Move create_move_take(Idx node_source_index, Idx node_destination_index, Idx node_take_index) {
        Move move;
        move.move_take.node_source_index = node_source_index;
        move.move_take.node_destination_index = node_destination_index;
        move.move_take.node_take_index = node_take_index;
        move.type = MoveType::MoveTake;

        return move;
    }

    static void get_moves_phase1(SearchCtx& ctx, Piece piece, Array<Move, MAX_MOVES>& moves) {
        assert(piece != Piece::None);

        for (IterIdx i = 0; i < NODES; i++) {
            if (ctx.board[i] != Piece::None) {
                continue;
            }

            make_place_move(ctx, piece, i);

            if (is_mill(ctx, piece, i)) {
                const Piece opponent = opponent_piece(piece);
                const bool all_in_mills = all_pieces_in_mills(ctx, opponent);

                for (IterIdx j = 0; j < NODES; j++) {
                    if (ctx.board[j] != opponent) {
                        continue;
                    }

                    if (is_mill(ctx, opponent, j) && !all_in_mills) {
                        continue;
                    }

                    moves.push_back(create_place_take(i, j));
                }
            } else {
                moves.push_back(create_place(i));
            }

            unmake_place_move(ctx, i);
        }
    }

    static void get_moves_phase2(SearchCtx& ctx, Piece piece, Array<Move, MAX_MOVES>& moves) {
        assert(piece != Piece::None);

        for (IterIdx i = 0; i < NODES; i++) {
            if (ctx.board[i] != piece) {
                continue;
            }

            const auto free_positions = neighbor_free_positions(ctx, i);

            for (IterIdx j = 0; j < free_positions.size(); j++) {
                make_move_move(ctx, piece, i, free_positions[j]);

                if (is_mill(ctx, piece, free_positions[j])) {
                    const auto opponent = opponent_piece(piece);
                    const bool all_in_mills = all_pieces_in_mills(ctx, opponent);

                    for (IterIdx k = 0; k < NODES; k++) {
                        if (ctx.board[k] != opponent) {
                            continue;
                        }

                        if (is_mill(ctx, opponent, k) && !all_in_mills) {
                            continue;
                        }

                        moves.push_back(create_move_take(i, free_positions[j], k));
                    }
                } else {
                    moves.push_back(create_move(i, free_positions[j]));
                }

                unmake_move_move(ctx, piece, i, free_positions[j]);
            }
        }
    }

    static void get_moves_phase3(SearchCtx& ctx, Piece piece, Array<Move, MAX_MOVES>& moves) {
        assert(piece != Piece::None);

        for (IterIdx i = 0; i < NODES; i++) {
            if (ctx.board[i] != piece) {
                continue;
            }

            for (IterIdx j = 0; j < NODES; j++) {
                if (ctx.board[j] != Piece::None) {
                    continue;
                }

                make_move_move(ctx, piece, i, j);

                if (is_mill(ctx, piece, j)) {
                    const auto opponent = opponent_piece(piece);
                    const bool all_in_mills = all_pieces_in_mills(ctx, opponent);

                    for (IterIdx k = 0; k < NODES; k++) {
                        if (ctx.board[k] != opponent) {
                            continue;
                        }

                        if (is_mill(ctx, opponent, k) && !all_in_mills) {
                            continue;
                        }

                        moves.push_back(create_move_take(i, j, k));
                    }
                } else {
                    moves.push_back(create_move(i, j));
                }

                unmake_move_move(ctx, piece, i, j);
            }
        }
    }

    void make_move(SearchCtx& ctx, const Move& move, Piece piece) {
        switch (move.type) {
            case MoveType::Place:
                ctx.board[move.place.node_index] = piece;

                switch (piece) {
                    case Piece::White:
                        ctx.white_pieces_on_board++;
                        break;
                    case Piece::Black:
                        ctx.black_pieces_on_board++;
                        break;
                    default:
                        break;
                }

                break;
            case MoveType::Move:
                ctx.board[move.move.node_source_index] = Piece::None;
                ctx.board[move.move.node_destination_index] = piece;

                break;
            case MoveType::PlaceTake:
                ctx.board[move.place_take.node_index] = piece;
                ctx.board[move.place_take.node_take_index] = Piece::None;

                switch (piece) {
                    case Piece::White:
                        ctx.black_pieces_on_board--;
                        break;
                    case Piece::Black:
                        ctx.white_pieces_on_board--;
                        break;
                    default:
                        break;
                }

                break;
            case MoveType::MoveTake:
                ctx.board[move.move_take.node_source_index] = Piece::None;
                ctx.board[move.move_take.node_destination_index] = piece;
                ctx.board[move.move_take.node_take_index] = Piece::None;

                switch (piece) {
                    case Piece::White:
                        ctx.black_pieces_on_board--;
                        break;
                    case Piece::Black:
                        ctx.white_pieces_on_board--;
                        break;
                    default:
                        break;
                }

                break;
        }

        ctx.plies++;
    }

    void unmake_move(SearchCtx& ctx, const Move& move, Piece piece) {
        switch (move.type) {
            case MoveType::Place:
                ctx.board[move.place.node_index] = Piece::None;

                switch (piece) {
                    case Piece::White:
                        ctx.white_pieces_on_board--;
                        break;
                    case Piece::Black:
                        ctx.black_pieces_on_board--;
                        break;
                    default:
                        break;
                }

                break;
            case MoveType::Move:
                ctx.board[move.move.node_source_index] = piece;
                ctx.board[move.move.node_destination_index] = Piece::None;

                break;
            case MoveType::PlaceTake:
                ctx.board[move.place_take.node_index] = Piece::None;
                ctx.board[move.place_take.node_take_index] = opponent_piece(piece);

                switch (piece) {
                    case Piece::White:
                        ctx.black_pieces_on_board++;
                        break;
                    case Piece::Black:
                        ctx.white_pieces_on_board++;
                        break;
                    default:
                        break;
                }

                break;
            case MoveType::MoveTake:
                ctx.board[move.move_take.node_source_index] = piece;
                ctx.board[move.move_take.node_destination_index] = Piece::None;
                ctx.board[move.move_take.node_take_index] = opponent_piece(piece);

                switch (piece) {
                    case Piece::White:
                        ctx.black_pieces_on_board++;
                        break;
                    case Piece::Black:
                        ctx.white_pieces_on_board++;
                        break;
                    default:
                        break;
                }

                break;
        }

        ctx.plies--;
    }

    void generate_moves(SearchCtx& ctx, Piece piece, Array<Move, MAX_MOVES>& moves) {
        if (ctx.plies < 18) {
            get_moves_phase1(ctx, piece, moves);
        } else {
            if (pieces_on_board(ctx, piece) == 3) {
                get_moves_phase3(ctx, piece, moves);
            } else {
                get_moves_phase2(ctx, piece, moves);
            }
        }
    }

    Move random_move(SearchCtx& ctx, Piece piece) {
        Array<Move, MAX_MOVES> moves;
        generate_moves(ctx, piece, moves);

        if (moves.empty()) {
            return {};
        }

        std::uniform_int_distribution<std::mt19937::result_type> distribution {0, moves.size() - 1};

        const auto random_index = distribution(random);

        return moves[random_index];
    }

    namespace repetition {
        bool check_current_node(const Board& board, Player turn, Node& current, const Node* previous) {
            const Position current_position = make_position_bitboard(board, turn);

            current.previous = previous;  // TODO cut previous nodes, if a take move occurs
            current.position = current_position;

            const Node* node = previous;
            unsigned int repetition = 1;

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

            // Iterate 21 times for the first part
            for (IterIdx i = 0; i < NODES - 3; i++) {
                switch (board[i]) {
                    case Piece::None:
                        // Nothing
                        break;
                    case Piece::White:
                        position.part1 |= White << i * 2;
                        break;
                    case Piece::Black:
                        position.part1 |= Black << i * 2;
                        break;
                }
            }

            // Last bit means the player
            position.part1 |= static_cast<std::uint64_t>(turn) << 63;

            // Iterate 3 times for the second part
            for (IterIdx i = 0; i < 3; i++) {
                switch (board[i + 21]) {
                    case Piece::None:
                        // Nothing
                        break;
                    case Piece::White:
                        position.part2 |= White << i * 2;
                        break;
                    case Piece::Black:
                        position.part2 |= Black << i * 2;
                        break;
                }
            }

            return position;
        }
    }
}
