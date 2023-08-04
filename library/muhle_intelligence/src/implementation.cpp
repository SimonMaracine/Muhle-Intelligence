#include <thread>
#include <unordered_map>
#include <array>
#include <string>
#include <string_view>
#include <climits>
#include <chrono>
#include <cassert>

#include "muhle_intelligence/internal/implementation.hpp"
#include "muhle_intelligence/internal/moves_array.hpp"
#include "muhle_intelligence/muhle_intelligence.hpp"

// TODO please don't optimize early!!!

/*
    Maximizing player is white.
    Minimizing player is black.

    Evaluation is a signed integer.
    Value of 0 means equal position, positive means white is better, negative means black is better.
*/

namespace muhle {
    static constexpr Eval MIN_EVALUATION = std::numeric_limits<Eval>::min();
    static constexpr Eval MAX_EVALUATION = std::numeric_limits<Eval>::max();

    void MuhleImpl::initialize() {
        // Here the default values are set
        parameters["PIECE"] = 7;
        parameters["FREEDOM"] = 1;
        parameters["END_GAME"] = 100;
        parameters["DEPTH"] = 5;
    }

    void MuhleImpl::search(const Position& position, Player player, Result& result) {
        result = {};

        thread = std::thread([this, position, player, &result]() {
            SearchCtx search;
            search.setup(parameters);
            search.figure_out_position(position);
            search.search(player, result);
        });
    }

    void MuhleImpl::join_thread() {
        thread.join();
    }

    void MuhleImpl::set_parameter(std::string_view parameter, int value) {
        parameters[std::string(parameter)] = value;
    }

    void SearchCtx::setup(const std::unordered_map<std::string, int>& parameters) {
        this->parameters.PIECE = parameters.at("PIECE");
        this->parameters.FREEDOM = parameters.at("FREEDOM");
        this->parameters.END_GAME = parameters.at("END_GAME");
        this->parameters.DEPTH = parameters.at("DEPTH");
    }

    void SearchCtx::figure_out_position(const Position& position) {
        this->position = position.pieces;

        for (IterIdx i = 0; i < NODES; i++) {
            switch (this->position[i]) {
                case Piece::White:
                    white_pieces_on_board++;
                    break;
                case Piece::Black:
                    black_pieces_on_board++;
                    break;
                default:
                    break;
            }
        }

        plies = position.plies;
    }

    void SearchCtx::search(Player player, Result& result) {
        const auto start = std::chrono::high_resolution_clock::now();

        const Eval evaluation = minimax(
            static_cast<unsigned int>(parameters.DEPTH),
            0u,
            MIN_EVALUATION,
            MAX_EVALUATION,
            player
        );

        const auto end = std::chrono::high_resolution_clock::now();

        const double time = std::chrono::duration<double>(end - start).count();

        result.result = best_move;
        result.time = time;
        result.evaluation = evaluation;
        result.positions_evaluated = positions_evaluated;

        result.done = true;
    }

    Eval SearchCtx::minimax(unsigned int depth, unsigned int plies_from_root, Eval alpha, Eval beta, Player player) {
        if (Eval evaluation_game_over = 0; depth == 0 || is_game_over(evaluation_game_over)) {
            return evaluate_position(evaluation_game_over);
        }

        if (player == Player::White) {
            Eval max_evaluation = MIN_EVALUATION;

            MovesArray<Move, MAX_MOVES> moves;
            get_all_moves(Piece::White, moves);

            for (const Move& move : moves) {
                make_move(move);
                const Eval evaluation = minimax(depth - 1, plies_from_root + 1, alpha, beta, Player::Black);
                unmake_move(move);

                if (evaluation > max_evaluation) {
                    max_evaluation = evaluation;

                    if (plies_from_root == 0) {
                        best_move = move;
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
        } else {
            Eval min_evaluation = MAX_EVALUATION;

            MovesArray<Move, MAX_MOVES> moves;
            get_all_moves(Piece::Black, moves);

            for (const Move& move : moves) {
                make_move(move);
                const Eval evaluation = minimax(depth - 1, plies_from_root + 1, alpha, beta, Player::White);
                unmake_move(move);

                if (evaluation < min_evaluation) {
                    min_evaluation = evaluation;

                    if (plies_from_root == 0) {
                        best_move = move;
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

    unsigned int SearchCtx::test_moves(Player player, unsigned int depth) {
        if (depth == 0) {
            return 1;
        }

        unsigned int move_count = 0;

        if (player == Player::White) {
            MovesArray<Move, MAX_MOVES> moves;
            get_all_moves(Piece::White, moves);

            for (const Move& move : moves) {
                make_move(move);
                move_count += test_moves(Player::Black, depth - 1);
                unmake_move(move);
            }
        } else {
            MovesArray<Move, MAX_MOVES> moves;
            get_all_moves(Piece::Black, moves);

            for (const Move& move : moves) {
                make_move(move);
                move_count += test_moves(Player::White, depth - 1);
                unmake_move(move);
            }
        }

        return move_count;
    }

    Move SearchCtx::random_move(Piece piece) {  // FIXME
        MovesArray<Move, MAX_MOVES> moves;
        get_all_moves(piece, moves);

        if (moves.empty()) {
            return {};
        }

        return moves[0];
    }

    void SearchCtx::get_all_moves(Piece piece, MovesArray<Move, MAX_MOVES>& moves) {
        if (plies < 18) {
            get_moves_phase1(piece, moves);
        } else {
            if (pieces_on_board(piece) == 3) {
                get_moves_phase3(piece, moves);
            } else {
                get_moves_phase2(piece, moves);
            }
        }
    }

    void SearchCtx::get_moves_phase1(Piece piece, MovesArray<Move, MAX_MOVES>& moves) {
        assert(piece != Piece::None);

        for (IterIdx i = 0; i < NODES; i++) {
            if (position[i] != Piece::None) {
                continue;
            }

            make_place_move(piece, i);

            if (is_mill(piece, i)) {
                const Piece opponent = opponent_piece(piece);
                const bool all_in_mills = all_pieces_in_mills(opponent);

                for (IterIdx j = 0; j < NODES; j++) {
                    if (position[j] != opponent) {
                        continue;
                    }

                    if (is_mill(opponent, j) && !all_in_mills) {
                        continue;
                    }

                    moves.push_back(create_place_take(piece, i, j));
                }
            } else {
                moves.push_back(create_place(piece, i));
            }

            unmake_place_move(i);
        }
    }

    void SearchCtx::get_moves_phase2(Piece piece, MovesArray<Move, MAX_MOVES>& moves) {
        assert(piece != Piece::None);

        for (IterIdx i = 0; i < NODES; i++) {
            if (position[i] != piece) {
                continue;
            }

            const auto free_positions = neighbor_free_positions(i);

            for (IterIdx j = 0; free_positions[j] != INVALID_INDEX; j++) {
                make_move_move(piece, i, free_positions[j]);

                if (is_mill(piece, free_positions[j])) {
                    const auto opponent = opponent_piece(piece);
                    const bool all_in_mills = all_pieces_in_mills(opponent);

                    for (IterIdx k = 0; k < NODES; k++) {
                        if (position[k] != opponent) {
                            continue;
                        }

                        if (is_mill(opponent, k) && !all_in_mills) {
                            continue;
                        }

                        moves.push_back(create_move_take(piece, i, free_positions[j], k));
                    }
                } else {
                    moves.push_back(create_move(piece, i, free_positions[j]));
                }

                unmake_move_move(piece, i, free_positions[j]);
            }
        }
    }

    void SearchCtx::get_moves_phase3(Piece piece, MovesArray<Move, MAX_MOVES>& moves) {
        assert(piece != Piece::None);

        for (IterIdx i = 0; i < NODES; i++) {
            if (position[i] != piece) {
                continue;
            }

            for (IterIdx j = 0; j < NODES; j++) {
                if (position[j] != Piece::None) {
                    continue;
                }

                make_move_move(piece, i, j);

                if (is_mill(piece, j)) {
                    const auto opponent = opponent_piece(piece);
                    const bool all_in_mills = all_pieces_in_mills(opponent);

                    for (IterIdx k = 0; k < NODES; k++) {
                        if (position[k] != opponent) {
                            continue;
                        }

                        if (is_mill(opponent, k) && !all_in_mills) {
                            continue;
                        }

                        moves.push_back(create_move_take(piece, i, j, k));
                    }
                } else {
                    moves.push_back(create_move(piece, i, j));
                }

                unmake_move_move(piece, i, j);
            }
        }
    }

    void SearchCtx::make_move(const Move& move) {
        switch (move.type) {
            case MoveType::Place:
                position[move.place.node_index] = move.piece;
                break;
            case MoveType::Move:
                position[move.move.node_destination_index] = move.piece;
                position[move.move.node_source_index] = Piece::None;
                break;
            case MoveType::PlaceTake:
                position[move.place_take.node_index] = move.piece;
                position[move.place_take.node_take_index] = Piece::None;
                break;
            case MoveType::MoveTake:
                position[move.move_take.node_destination_index] = move.piece;
                position[move.move_take.node_source_index] = Piece::None;
                position[move.move_take.node_take_index] = Piece::None;
                break;
        }

        plies++;
    }

    void SearchCtx::unmake_move(const Move& move) {
        switch (move.type) {
            case MoveType::Place:
                position[move.place.node_index] = Piece::None;
                break;
            case MoveType::Move:
                position[move.move.node_destination_index] = Piece::None;
                position[move.move.node_source_index] = move.piece;
                break;
            case MoveType::PlaceTake:
                position[move.place_take.node_index] = Piece::None;
                position[move.place_take.node_take_index] = opponent_piece(move.piece);
                break;
            case MoveType::MoveTake:
                position[move.move_take.node_destination_index] = Piece::None;
                position[move.move_take.node_source_index] = move.piece;
                position[move.move_take.node_take_index] = opponent_piece(move.piece);
                break;
        }

        plies--;
    }

    void SearchCtx::make_place_move(Piece piece, Idx node_index) {
        position[node_index] = piece;
    }

    void SearchCtx::unmake_place_move(Idx node_index) {
        position[node_index] = Piece::None;
    }

    void SearchCtx::make_move_move(Piece piece, Idx node_source_index, Idx node_destination_index) {
        position[node_source_index] = Piece::None;
        position[node_destination_index] = piece;
    }

    void SearchCtx::unmake_move_move(Piece piece, Idx node_source_index, Idx node_destination_index) {
        position[node_source_index] = piece;
        position[node_destination_index] = Piece::None;
    }

    Eval SearchCtx::evaluate_position(Eval evaluation_game_over) {  // TODO also evaluate piece positions
        positions_evaluated++;

        Eval evaluation = 0;

        const Eval evaluation_material = calculate_material_both();

        // Calculate number of pieces
        evaluation += evaluation_material * static_cast<Eval>(parameters.PIECE);

        const Eval evaluation_freedom = calculate_freedom_both();

        // Calculate pieces' freedom
        evaluation += evaluation_freedom * static_cast<Eval>(parameters.FREEDOM);

        // Teach what is end game
        evaluation += evaluation_game_over * static_cast<Eval>(parameters.END_GAME);

        return evaluation;
    }

    unsigned int SearchCtx::calculate_material(Piece piece) {
        assert(piece != Piece::None);

        unsigned int piece_count = 0;

        for (IterIdx i = 0; i < NODES; i++) {
            piece_count += position[i] == piece;
        }

        return piece_count;
    }

    Eval SearchCtx::calculate_material_both() {
        Eval evaluation_material = 0;

        for (IterIdx i = 0; i < NODES; i++) {
            evaluation_material += static_cast<Eval>(position[i]);
        }

        return evaluation_material;
    }

    void SearchCtx::calculate_freedom_both(unsigned int& white, unsigned int& black) {
        unsigned int total_white_free_positions = 0;
        unsigned int total_black_free_positions = 0;

        for (IterIdx i = 0; i < NODES; i++) {
            switch (position[i]) {
                case Piece::White:
                    total_white_free_positions += calculate_piece_freedom(i);
                    break;
                case Piece::Black:
                    total_black_free_positions += calculate_piece_freedom(i);
                    break;
                default:
                    break;
            }
        }

        white = total_white_free_positions;
        black = total_black_free_positions;
    }

    unsigned int SearchCtx::calculate_piece_freedom(Idx index) {
        assert(position[index] != Piece::None);

        unsigned int freedom = 0;

        switch (index) {
            case 0:
                freedom += position[1] == Piece::None;
                freedom += position[9] == Piece::None;
                break;
            case 1:
                freedom += position[0] == Piece::None;
                freedom += position[2] == Piece::None;
                freedom += position[4] == Piece::None;
                break;
            case 2:
                freedom += position[1] == Piece::None;
                freedom += position[14] == Piece::None;
                break;
            case 3:
                freedom += position[4] == Piece::None;
                freedom += position[10] == Piece::None;
                break;
            case 4:
                freedom += position[1] == Piece::None;
                freedom += position[3] == Piece::None;
                freedom += position[5] == Piece::None;
                freedom += position[7] == Piece::None;
                break;
            case 5:
                freedom += position[4] == Piece::None;
                freedom += position[13] == Piece::None;
                break;
            case 6:
                freedom += position[7] == Piece::None;
                freedom += position[11] == Piece::None;
                break;
            case 7:
                freedom += position[4] == Piece::None;
                freedom += position[6] == Piece::None;
                freedom += position[8] == Piece::None;
                break;
            case 8:
                freedom += position[7] == Piece::None;
                freedom += position[12] == Piece::None;
                break;
            case 9:
                freedom += position[0] == Piece::None;
                freedom += position[10] == Piece::None;
                freedom += position[21] == Piece::None;
                break;
            case 10:
                freedom += position[3] == Piece::None;
                freedom += position[9] == Piece::None;
                freedom += position[11] == Piece::None;
                freedom += position[18] == Piece::None;
                break;
            case 11:
                freedom += position[6] == Piece::None;
                freedom += position[10] == Piece::None;
                freedom += position[15] == Piece::None;
                break;
            case 12:
                freedom += position[8] == Piece::None;
                freedom += position[13] == Piece::None;
                freedom += position[17] == Piece::None;
                break;
            case 13:
                freedom += position[5] == Piece::None;
                freedom += position[12] == Piece::None;
                freedom += position[14] == Piece::None;
                freedom += position[20] == Piece::None;
                break;
            case 14:
                freedom += position[2] == Piece::None;
                freedom += position[13] == Piece::None;
                freedom += position[23] == Piece::None;
                break;
            case 15:
                freedom += position[11] == Piece::None;
                freedom += position[16] == Piece::None;
                break;
            case 16:
                freedom += position[15] == Piece::None;
                freedom += position[17] == Piece::None;
                freedom += position[19] == Piece::None;
                break;
            case 17:
                freedom += position[12] == Piece::None;
                freedom += position[16] == Piece::None;
                break;
            case 18:
                freedom += position[10] == Piece::None;
                freedom += position[19] == Piece::None;
                break;
            case 19:
                freedom += position[16] == Piece::None;
                freedom += position[18] == Piece::None;
                freedom += position[20] == Piece::None;
                freedom += position[22] == Piece::None;
                break;
            case 20:
                freedom += position[13] == Piece::None;
                freedom += position[19] == Piece::None;
                break;
            case 21:
                freedom += position[9] == Piece::None;
                freedom += position[22] == Piece::None;
                break;
            case 22:
                freedom += position[19] == Piece::None;
                freedom += position[21] == Piece::None;
                freedom += position[23] == Piece::None;
                break;
            case 23:
                freedom += position[14] == Piece::None;
                freedom += position[22] == Piece::None;
                break;
        }

        return freedom;
    }

    Eval SearchCtx::calculate_freedom_both() {
        Eval evaluation_freedom = 0;

        for (IterIdx i = 0; i < NODES; i++) {
            evaluation_freedom += (
                static_cast<Eval>(calculate_piece_freedom(i)) * static_cast<Eval>(position[i])
            );
        }

        return evaluation_freedom;
    }

    bool SearchCtx::all_pieces_in_mills(Piece piece) {
        for (IterIdx i = 0; i < NODES; i++) {
            if (position[i] != piece) {
                continue;
            }

            if (!is_mill(piece, i)) {
                return false;
            }
        }

        return true;
    }

#define IS_FREE_CHECK(const_index) \
    if (position[const_index] == Piece::None) { \
        result[pos++] = (const_index); \
    }

    std::array<Idx, 5> SearchCtx::neighbor_free_positions(Idx index) {
        std::array<Idx, 5> result = {
            INVALID_INDEX, INVALID_INDEX, INVALID_INDEX, INVALID_INDEX, INVALID_INDEX
        };
        unsigned int pos = 0;

        switch (index) {
            case 0:
                IS_FREE_CHECK(1)
                IS_FREE_CHECK(9)
                break;
            case 1:
                IS_FREE_CHECK(0)
                IS_FREE_CHECK(2)
                IS_FREE_CHECK(4)
                break;
            case 2:
                IS_FREE_CHECK(1)
                IS_FREE_CHECK(14)
                break;
            case 3:
                IS_FREE_CHECK(4)
                IS_FREE_CHECK(10)
                break;
            case 4:
                IS_FREE_CHECK(1)
                IS_FREE_CHECK(3)
                IS_FREE_CHECK(5)
                IS_FREE_CHECK(7)
                break;
            case 5:
                IS_FREE_CHECK(4)
                IS_FREE_CHECK(13)
                break;
            case 6:
                IS_FREE_CHECK(7)
                IS_FREE_CHECK(11)
                break;
            case 7:
                IS_FREE_CHECK(4)
                IS_FREE_CHECK(6)
                IS_FREE_CHECK(8)
                break;
            case 8:
                IS_FREE_CHECK(7)
                IS_FREE_CHECK(12)
                break;
            case 9:
                IS_FREE_CHECK(0)
                IS_FREE_CHECK(10)
                IS_FREE_CHECK(21)
                break;
            case 10:
                IS_FREE_CHECK(3)
                IS_FREE_CHECK(9)
                IS_FREE_CHECK(11)
                IS_FREE_CHECK(18)
                break;
            case 11:
                IS_FREE_CHECK(6)
                IS_FREE_CHECK(10)
                IS_FREE_CHECK(15)
                break;
            case 12:
                IS_FREE_CHECK(8)
                IS_FREE_CHECK(13)
                IS_FREE_CHECK(17)
                break;
            case 13:
                IS_FREE_CHECK(5)
                IS_FREE_CHECK(12)
                IS_FREE_CHECK(14)
                IS_FREE_CHECK(20)
                break;
            case 14:
                IS_FREE_CHECK(2)
                IS_FREE_CHECK(13)
                IS_FREE_CHECK(23)
                break;
            case 15:
                IS_FREE_CHECK(11)
                IS_FREE_CHECK(16)
                break;
            case 16:
                IS_FREE_CHECK(15)
                IS_FREE_CHECK(17)
                IS_FREE_CHECK(19)
                break;
            case 17:
                IS_FREE_CHECK(12)
                IS_FREE_CHECK(16)
                break;
            case 18:
                IS_FREE_CHECK(10)
                IS_FREE_CHECK(19)
                break;
            case 19:
                IS_FREE_CHECK(16)
                IS_FREE_CHECK(18)
                IS_FREE_CHECK(20)
                IS_FREE_CHECK(22)
                break;
            case 20:
                IS_FREE_CHECK(13)
                IS_FREE_CHECK(19)
                break;
            case 21:
                IS_FREE_CHECK(9)
                IS_FREE_CHECK(22)
                break;
            case 22:
                IS_FREE_CHECK(19)
                IS_FREE_CHECK(21)
                IS_FREE_CHECK(23)
                break;
            case 23:
                IS_FREE_CHECK(14)
                IS_FREE_CHECK(22)
                break;
        }

        return result;
    }

    Piece SearchCtx::opponent_piece(Piece type) {
        assert(type != Piece::None);

        switch (type) {
            case Piece::White:
                return Piece::Black;
            case Piece::Black:
                return Piece::White;
            default:
                break;
        }

        return {};
    }

#define IS_PC(const_index) (position[const_index] == piece)

    bool SearchCtx::is_mill(Piece piece, Idx index) {
        assert(piece != Piece::None);

        switch (index) {
            case 0:
                if (IS_PC(1) && IS_PC(2) || IS_PC(9) && IS_PC(21))
                    return true;
                break;
            case 1:
                if (IS_PC(0) && IS_PC(2) || IS_PC(4) && IS_PC(7))
                    return true;
                break;
            case 2:
                if (IS_PC(0) && IS_PC(1) || IS_PC(14) && IS_PC(23))
                    return true;
                break;
            case 3:
                if (IS_PC(4) && IS_PC(5) || IS_PC(10) && IS_PC(18))
                    return true;
                break;
            case 4:
                if (IS_PC(3) && IS_PC(5) || IS_PC(1) && IS_PC(7))
                    return true;
                break;
            case 5:
                if (IS_PC(3) && IS_PC(4) || IS_PC(13) && IS_PC(20))
                    return true;
                break;
            case 6:
                if (IS_PC(7) && IS_PC(8) || IS_PC(11) && IS_PC(15))
                    return true;
                break;
            case 7:
                if (IS_PC(6) && IS_PC(8) || IS_PC(1) && IS_PC(4))
                    return true;
                break;
            case 8:
                if (IS_PC(6) && IS_PC(7) || IS_PC(12) && IS_PC(17))
                    return true;
                break;
            case 9:
                if (IS_PC(0) && IS_PC(21) || IS_PC(10) && IS_PC(11))
                    return true;
                break;
            case 10:
                if (IS_PC(9) && IS_PC(11) || IS_PC(3) && IS_PC(18))
                    return true;
                break;
            case 11:
                if (IS_PC(9) && IS_PC(10) || IS_PC(6) && IS_PC(15))
                    return true;
                break;
            case 12:
                if (IS_PC(13) && IS_PC(14) || IS_PC(8) && IS_PC(17))
                    return true;
                break;
            case 13:
                if (IS_PC(12) && IS_PC(14) || IS_PC(5) && IS_PC(20))
                    return true;
                break;
            case 14:
                if (IS_PC(12) && IS_PC(13) || IS_PC(2) && IS_PC(23))
                    return true;
                break;
            case 15:
                if (IS_PC(16) && IS_PC(17) || IS_PC(6) && IS_PC(11))
                    return true;
                break;
            case 16:
                if (IS_PC(15) && IS_PC(17) || IS_PC(19) && IS_PC(22))
                    return true;
                break;
            case 17:
                if (IS_PC(15) && IS_PC(16) || IS_PC(8) && IS_PC(12))
                    return true;
                break;
            case 18:
                if (IS_PC(19) && IS_PC(20) || IS_PC(3) && IS_PC(10))
                    return true;
                break;
            case 19:
                if (IS_PC(18) && IS_PC(20) || IS_PC(16) && IS_PC(22))
                    return true;
                break;
            case 20:
                if (IS_PC(18) && IS_PC(19) || IS_PC(5) && IS_PC(13))
                    return true;
                break;
            case 21:
                if (IS_PC(22) && IS_PC(23) || IS_PC(0) && IS_PC(9))
                    return true;
                break;
            case 22:
                if (IS_PC(21) && IS_PC(23) || IS_PC(16) && IS_PC(19))
                    return true;
                break;
            case 23:
                if (IS_PC(21) && IS_PC(22) || IS_PC(2) && IS_PC(14))
                    return true;
                break;
        }

        return false;
    }

    bool SearchCtx::is_game_over(Eval& evaluation_game_over) {
        if (plies < 18) {
            return false;
        }

        if (white_pieces_on_board < 3) {
            evaluation_game_over = -1;
            return true;
        }

        if (black_pieces_on_board < 3) {
            evaluation_game_over = 1;
            return true;
        }

        unsigned int white_free_positions = 0;
        unsigned int black_free_positions = 0;
        calculate_freedom_both(white_free_positions, black_free_positions);

        if (white_free_positions == 0) {
            evaluation_game_over = -1;
            return true;
        }

        if (black_free_positions == 0) {
            evaluation_game_over = 1;
            return true;
        }

        return false;
    }

    unsigned int SearchCtx::pieces_on_board(Piece piece) {
        assert(piece != Piece::None);

        switch (piece) {
            case Piece::White:
                return white_pieces_on_board;
            case Piece::Black:
                return black_pieces_on_board;
            default:
                break;
        }

        return 0;
    }

    Move create_place(Piece piece, Idx node_index) {
        Move move;
        move.place.node_index = node_index;
        move.type = MoveType::Place;
        move.piece = piece;

        return move;
    }

    Move create_move(Piece piece, Idx node_source_index, Idx node_destination_index) {
        Move move;
        move.move.node_source_index = node_source_index;
        move.move.node_destination_index = node_destination_index;
        move.type = MoveType::Move;
        move.piece = piece;

        return move;
    }

    Move create_place_take(Piece piece, Idx node_index, Idx node_take_index) {
        Move move;
        move.place_take.node_index = node_index;
        move.place_take.node_take_index = node_take_index;
        move.type = MoveType::PlaceTake;
        move.piece = piece;

        return move;
    }

    Move create_move_take(Piece piece, Idx node_source_index, Idx node_destination_index, Idx node_take_index) {
        Move move;
        move.move_take.node_source_index = node_source_index;
        move.move_take.node_destination_index = node_destination_index;
        move.move_take.node_take_index = node_take_index;
        move.type = MoveType::MoveTake;
        move.piece = piece;

        return move;
    }
}
