#include <cstddef>
#include <forward_list>
#include <array>
#include <tuple>
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
        ctx.position[node_index] = piece;
    }

    static void unmake_place_move(SearchCtx& ctx, Idx node_index) {
        ctx.position[node_index] = Piece::None;
    }

    static void make_move_move(SearchCtx& ctx, Piece piece, Idx node_source_index, Idx node_destination_index) {
        ctx.position[node_source_index] = Piece::None;
        ctx.position[node_destination_index] = piece;
    }

    static void unmake_move_move(SearchCtx& ctx, Piece piece, Idx node_source_index, Idx node_destination_index) {
        ctx.position[node_source_index] = piece;
        ctx.position[node_destination_index] = Piece::None;
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
            if (ctx.position[i] != Piece::None) {
                continue;
            }

            make_place_move(ctx, piece, i);

            if (is_mill(ctx, piece, i)) {
                const Piece opponent = opponent_piece(piece);
                const bool all_in_mills = all_pieces_in_mills(ctx, opponent);

                for (IterIdx j = 0; j < NODES; j++) {
                    if (ctx.position[j] != opponent) {
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
            if (ctx.position[i] != piece) {
                continue;
            }

            const auto free_positions = neighbor_free_positions(ctx, i);

            for (IterIdx j = 0; j < free_positions.size(); j++) {
                make_move_move(ctx, piece, i, free_positions[j]);

                if (is_mill(ctx, piece, free_positions[j])) {
                    const auto opponent = opponent_piece(piece);
                    const bool all_in_mills = all_pieces_in_mills(ctx, opponent);

                    for (IterIdx k = 0; k < NODES; k++) {
                        if (ctx.position[k] != opponent) {
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
            if (ctx.position[i] != piece) {
                continue;
            }

            for (IterIdx j = 0; j < NODES; j++) {
                if (ctx.position[j] != Piece::None) {
                    continue;
                }

                make_move_move(ctx, piece, i, j);

                if (is_mill(ctx, piece, j)) {
                    const auto opponent = opponent_piece(piece);
                    const bool all_in_mills = all_pieces_in_mills(ctx, opponent);

                    for (IterIdx k = 0; k < NODES; k++) {
                        if (ctx.position[k] != opponent) {
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
                ctx.position[move.place.node_index] = piece;

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
                ctx.position[move.move.node_source_index] = Piece::None;
                ctx.position[move.move.node_destination_index] = piece;

                break;
            case MoveType::PlaceTake:
                ctx.position[move.place_take.node_index] = piece;
                ctx.position[move.place_take.node_take_index] = Piece::None;

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
                ctx.position[move.move_take.node_source_index] = Piece::None;
                ctx.position[move.move_take.node_destination_index] = piece;
                ctx.position[move.move_take.node_take_index] = Piece::None;

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
                ctx.position[move.place.node_index] = Piece::None;

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
                ctx.position[move.move.node_source_index] = piece;
                ctx.position[move.move.node_destination_index] = Piece::None;

                break;
            case MoveType::PlaceTake:
                ctx.position[move.place_take.node_index] = Piece::None;
                ctx.position[move.place_take.node_take_index] = opponent_piece(piece);

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
                ctx.position[move.move_take.node_source_index] = piece;
                ctx.position[move.move_take.node_destination_index] = Piece::None;
                ctx.position[move.move_take.node_take_index] = opponent_piece(piece);

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

    bool ThreefoldRepetition::threefold_repetition() {

    }

    void ThreefoldRepetition::clear_repetition() {
        ones.clear();
        twos.clear();
    }

    ThreefoldRepetition::Position ThreefoldRepetition::write_position(const std::array<Piece, NODES>& pieces, Player turn) {
        std::uint64_t position1 = 0;
        std::uint16_t position2 = 0;

        for (IterIdx i = 0; i < NODES; i++) {
            switch (pieces[i]) {
                case Piece::Black:
                    // const std::uint64_t mask = 1 <<

                    break;
                case Piece::None:
                    break;
                case Piece::White:
                    break;
            }



        }

        return {};
    }

    std::pair<std::array<Piece, NODES>, Player> ThreefoldRepetition::read_position(Position position) {
        return {};
    }
}
