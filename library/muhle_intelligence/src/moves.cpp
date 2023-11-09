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
    static void make_place_move(Board& board, Piece piece, Idx node_index) {
        board[node_index] = piece;
    }

    static void unmake_place_move(Board& board, Idx node_index) {
        board[node_index] = Piece::None;
    }

    static void make_move_move(Board& board, Piece piece, Idx node_source_index, Idx node_destination_index) {
        board[node_source_index] = Piece::None;
        board[node_destination_index] = piece;
    }

    static void unmake_move_move(Board& board, Piece piece, Idx node_source_index, Idx node_destination_index) {
        board[node_source_index] = piece;
        board[node_destination_index] = Piece::None;
    }

    static Move create_place(Idx node_index) {
        Move move;
        move.place.place_index = node_index;
        move.type = MoveType::Place;

        return move;
    }

    static Move create_move(Idx node_source_index, Idx node_destination_index) {
        Move move;
        move.move.source_index = node_source_index;
        move.move.destination_index = node_destination_index;
        move.type = MoveType::Move;

        return move;
    }

    static Move create_place_take(Idx node_index, Idx node_take_index) {
        Move move;
        move.place_take.place_index = node_index;
        move.place_take.take_index = node_take_index;
        move.type = MoveType::PlaceTake;

        return move;
    }

    static Move create_move_take(Idx node_source_index, Idx node_destination_index, Idx node_take_index) {
        Move move;
        move.move_take.source_index = node_source_index;
        move.move_take.destination_index = node_destination_index;
        move.move_take.take_index = node_take_index;
        move.type = MoveType::MoveTake;

        return move;
    }

    static void get_moves_phase1(Board& board, Piece piece, Moves& moves) {
        assert(piece != Piece::None);

        for (IterIdx i {0}; i < NODES; i++) {
            if (board[i] != Piece::None) {
                continue;
            }

            make_place_move(board, piece, i);

            if (is_mill(board, piece, i)) {
                const Piece opponent {opponent_piece(piece)};
                const bool all_in_mills {all_pieces_in_mills(board, opponent)};

                for (IterIdx j {0}; j < NODES; j++) {
                    if (board[j] != opponent) {
                        continue;
                    }

                    if (is_mill(board, opponent, j) && !all_in_mills) {
                        continue;
                    }

                    moves.push_back(create_place_take(i, j));
                }
            } else {
                moves.push_back(create_place(i));
            }

            unmake_place_move(board, i);
        }
    }

    static void get_moves_phase2(Board& board, Piece piece, Moves& moves) {
        assert(piece != Piece::None);

        for (IterIdx i {0}; i < NODES; i++) {
            if (board[i] != piece) {
                continue;
            }

            const auto free_positions {neighbor_free_positions(board, i)};

            for (IterIdx j {0}; j < static_cast<IterIdx>(free_positions.size()); j++) {
                make_move_move(board, piece, i, free_positions[j]);

                if (is_mill(board, piece, free_positions[j])) {
                    const auto opponent {opponent_piece(piece)};
                    const bool all_in_mills {all_pieces_in_mills(board, opponent)};

                    for (IterIdx k {0}; k < NODES; k++) {
                        if (board[k] != opponent) {
                            continue;
                        }

                        if (is_mill(board, opponent, k) && !all_in_mills) {
                            continue;
                        }

                        moves.push_back(create_move_take(i, free_positions[j], k));
                    }
                } else {
                    moves.push_back(create_move(i, free_positions[j]));
                }

                unmake_move_move(board, piece, i, free_positions[j]);
            }
        }
    }

    static void get_moves_phase3(Board& board, Piece piece, Moves& moves) {
        assert(piece != Piece::None);

        for (IterIdx i {0}; i < NODES; i++) {
            if (board[i] != piece) {
                continue;
            }

            for (IterIdx j {0}; j < NODES; j++) {
                if (board[j] != Piece::None) {
                    continue;
                }

                make_move_move(board, piece, i, j);

                if (is_mill(board, piece, j)) {
                    const auto opponent {opponent_piece(piece)};
                    const bool all_in_mills {all_pieces_in_mills(board, opponent)};

                    for (IterIdx k {0}; k < NODES; k++) {
                        if (board[k] != opponent) {
                            continue;
                        }

                        if (is_mill(board, opponent, k) && !all_in_mills) {
                            continue;
                        }

                        moves.push_back(create_move_take(i, j, k));
                    }
                } else {
                    moves.push_back(create_move(i, j));
                }

                unmake_move_move(board, piece, i, j);
            }
        }
    }

    void play_move(SearchNode& node, const Move& move, Piece piece) {
        switch (move.type) {
            case MoveType::Place:
                node.board[move.place.place_index] = piece;

                switch (piece) {
                    case Piece::White:
                        node.white_pieces_on_board++;
                        break;
                    case Piece::Black:
                        node.black_pieces_on_board++;
                        break;
                    default:
                        break;
                }

                break;
            case MoveType::Move:
                node.board[move.move.source_index] = Piece::None;
                node.board[move.move.destination_index] = piece;

                break;
            case MoveType::PlaceTake:
                node.board[move.place_take.place_index] = piece;
                node.board[move.place_take.take_index] = Piece::None;

                switch (piece) {
                    case Piece::White:
                        node.black_pieces_on_board--;
                        break;
                    case Piece::Black:
                        node.white_pieces_on_board--;
                        break;
                    default:
                        break;
                }

                break;
            case MoveType::MoveTake:
                node.board[move.move_take.source_index] = Piece::None;
                node.board[move.move_take.destination_index] = piece;
                node.board[move.move_take.take_index] = Piece::None;

                switch (piece) {
                    case Piece::White:
                        node.black_pieces_on_board--;
                        break;
                    case Piece::Black:
                        node.white_pieces_on_board--;
                        break;
                    default:
                        break;
                }

                break;
        }

        node.plies++;
        node.rep_position = (
            std::make_optional(
                repetition::make_position_bitboard(
                    node.board,
                    piece == Piece::White ? Player::White : Player::Black  // TODO
                )
            )
        );

        if (is_take_move(move)) {
            node.plies_without_mills = 0;
            node.previous = nullptr;
        } else {
            node.plies_without_mills++;
        }
    }

    // void unmake_move(SearchNode& node, const Move& move, Piece piece) {
    //     switch (move.type) {
    //         case MoveType::Place:
    //             node.board[move.place.place_index] = Piece::None;

    //             switch (piece) {
    //                 case Piece::White:
    //                     node.white_pieces_on_board--;
    //                     break;
    //                 case Piece::Black:
    //                     node.black_pieces_on_board--;
    //                     break;
    //                 default:
    //                     break;
    //             }

    //             break;
    //         case MoveType::Move:
    //             node.board[move.move.source_index] = piece;
    //             node.board[move.move.destination_index] = Piece::None;

    //             break;
    //         case MoveType::PlaceTake:
    //             node.board[move.place_take.place_index] = Piece::None;
    //             node.board[move.place_take.take_index] = opponent_piece(piece);

    //             switch (piece) {
    //                 case Piece::White:
    //                     node.black_pieces_on_board++;
    //                     break;
    //                 case Piece::Black:
    //                     node.white_pieces_on_board++;
    //                     break;
    //                 default:
    //                     break;
    //             }

    //             break;
    //         case MoveType::MoveTake:
    //             node.board[move.move_take.source_index] = piece;
    //             node.board[move.move_take.destination_index] = Piece::None;
    //             node.board[move.move_take.take_index] = opponent_piece(piece);

    //             switch (piece) {
    //                 case Piece::White:
    //                     node.black_pieces_on_board++;
    //                     break;
    //                 case Piece::Black:
    //                     node.white_pieces_on_board++;
    //                     break;
    //                 default:
    //                     break;
    //             }

    //             break;
    //     }

    //     node.plies--;
    // }

    void play_move(SmnPosition& position, const Move& move) {
        static const Piece pieces[2] {Piece::White, Piece::Black};

        const Piece piece {pieces[static_cast<int>(position.position.player)]};

        switch (move.type) {
            case MoveType::Place:
                position.position.board[move.place.place_index] = piece;
                break;
            case MoveType::Move:
                position.position.board[move.move.source_index] = Piece::None;
                position.position.board[move.move.destination_index] = piece;
                break;
            case MoveType::PlaceTake:
                position.position.board[move.place_take.place_index] = piece;
                position.position.board[move.place_take.take_index] = Piece::None;
                break;
            case MoveType::MoveTake:
                position.position.board[move.move_take.source_index] = Piece::None;
                position.position.board[move.move_take.destination_index] = piece;
                position.position.board[move.move_take.take_index] = Piece::None;
                break;
        }

        position.position.player = opponent(position.position.player);
        position.plies++;
    }

    void generate_moves(SearchNode& node, Piece piece, Moves& moves) {
        if (node.plies < 18) {
            get_moves_phase1(node.board, piece, moves);
        } else {
            if (pieces_on_board(node, piece) == 3) {
                get_moves_phase3(node.board, piece, moves);
            } else {
                get_moves_phase2(node.board, piece, moves);
            }
        }
    }

    Move random_move(SearchNode& node, Piece piece) {
        std::random_device device;
        static std::mt19937 random {std::mt19937(device())};

        Moves moves;
        generate_moves(node, piece, moves);

        if (moves.empty()) {
            return {};
        }

        std::uniform_int_distribution<std::mt19937::result_type> distribution {0, moves.size() - 1};

        const auto random_index {distribution(random)};

        return moves[random_index];
    }

    bool is_take_move(const Move& move) {
        return static_cast<signed char>(move.type) > 1;
    }
}
