#include <thread>
#include <vector>
#include <unordered_map>
#include <string>
#include <string_view>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <climits>
#include <chrono>
#include <utility>

#include "muhle_intelligence/internal/implementation.hpp"
#include "muhle_intelligence/muhle_intelligence.hpp"

// TODO please don't optimize early!!!

/*
    Maximizing player is white.
    Minimizing player is black.

    Evaluation is a signed integer.
    Value of 0 means equal position, positive means white is better, negative means black is better.
*/

namespace muhle {
    static constexpr int MIN_EVALUATION = std::numeric_limits<int>::min();
    static constexpr int MAX_EVALUATION = std::numeric_limits<int>::max();

    void MuhleImpl::initialize() {
        // Here the default values are set
        parameters["PIECE"] = 7;
        parameters["FREEDOM"] = 1;
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
        this->parameters.DEPTH = parameters.at("DEPTH");
    }

    void SearchCtx::figure_out_position(const Position& position) {
        this->position = position.pieces;

        for (int i = 0; i < NODES; i++) {
            if (this->position[i] == Piece::White) {
                white_pieces_on_board++;
            } else if (this->position[i] == Piece::Black) {
                black_pieces_on_board++;
            }
        }

        white_pieces_outside = position.white_pieces_outside;
        black_pieces_outside = position.black_pieces_outside;
    }

    void SearchCtx::search(Player player, Result& result) {
        auto start = std::chrono::high_resolution_clock::now();
        evaluation = minimax(parameters.DEPTH, 0, MIN_EVALUATION, MAX_EVALUATION, player);
        auto end = std::chrono::high_resolution_clock::now();

        std::cout << "Time: " << std::setprecision(3) << std::chrono::duration<double>(end - start).count() << '\n';
        std::cout << "Evaluation: " << evaluation << '\n';
        std::cout << "Positions evaluated: " << positions_evaluated << '\n';
        std::cout << "Depth searched: " << parameters.DEPTH << '\n';

        result.result = best_move;

        result.done = true;
    }

    int SearchCtx::minimax(unsigned int depth, unsigned int plies_from_root, int alpha, int beta, Player player) {
        if (depth == 0 || is_game_over()) {
            return evaluate_position();
        }

        if (player == Player::White) {
            int max_evaluation = MIN_EVALUATION;

            const auto moves = get_all_moves(Piece::White);

            for (const Move& move : moves) {
                make_move(move);
                const int evaluation = minimax(depth - 1, plies_from_root + 1, alpha, beta, Player::Black);
                unmake_move(move);

                if (evaluation > max_evaluation) {
                    max_evaluation = evaluation;

                    if (plies_from_root == 0) {
                        best_move = move;  // TODO don't copy
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
            int min_evaluation = MAX_EVALUATION;

            const auto moves = get_all_moves(Piece::Black);

            for (const Move& move : moves) {
                make_move(move);
                const int evaluation = minimax(depth - 1, plies_from_root + 1, alpha, beta, Player::White);
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

    Move SearchCtx::random_move(Piece piece) {
        const auto moves = get_all_moves(piece);

        for (const auto& move : moves) {
            std::cout << move << '\n';
        }
        std::cout << std::endl;

        if (moves.empty()) {
            std::cout << "Game Over\n";
            return {};
        }

        return moves[0];  // TODO
    }

    std::vector<Move> SearchCtx::get_all_moves(Piece piece) {
        std::vector<Move> moves;  // TODO optimize allocation
        moves.reserve(54);  // This is the maximum amount of moves possible

        if (total_number_of_pieces(piece) == 3) {  // Phase 3
            get_moves_phase3(piece, moves);

            assert(moves.size() <= 54);

            return moves;
        }

        if (white_pieces_outside + black_pieces_outside > 0) {  // Phase 1
            get_moves_phase1(piece, moves);
        } else {  // Phase 2
            get_moves_phase2(piece, moves);
        }

        assert(moves.size() <= 54);

        return moves;
    }

    void SearchCtx::get_moves_phase1(Piece piece, std::vector<Move>& moves) {
        assert(piece != Piece::None);

        for (int i = 0; i < NODES; i++) {
            if (position[i] == Piece::None) {
                make_place_move(piece, i);

                if (is_mill(piece, i)) {
                    const auto opponent = opponent_piece(piece);

                    for (int j = 0; j < NODES; j++) {
                        if (position[j] == opponent) {
                            if (is_mill(opponent, j) && !all_pieces_in_mills(opponent)) {
                                continue;
                            }

                            moves.push_back(create_place_take(piece, i, j));
                        }
                    }
                } else {
                    moves.push_back(create_place(piece, i));
                }

                unmake_place_move(i);
            }
        }
    }

    void SearchCtx::get_moves_phase2(Piece piece, std::vector<Move>& moves) {
        assert(piece != Piece::None);

        for (int i = 0; i < NODES; i++) {
            if (position[i] == piece) {
                const auto free_positions = neighbor_free_positions(i);

                for (int j = 0; free_positions[j] != -1; j++) {
                    make_move_move(piece, i, free_positions[j]);

                    if (is_mill(piece, free_positions[j])) {
                        const auto opponent = opponent_piece(piece);

                        for (int k = 0; k < NODES; k++) {
                            if (position[k] == opponent) {
                                if (is_mill(opponent, k) && !all_pieces_in_mills(opponent)) {
                                    continue;
                                }

                                moves.push_back(create_move_take(piece, i, free_positions[j], k));
                            }
                        }
                    } else {
                        moves.push_back(create_move(piece, i, free_positions[j]));
                    }

                    unmake_move_move(piece, i, free_positions[j]);
                }
            }
        }
    }

    void SearchCtx::get_moves_phase3(Piece piece, std::vector<Move>& moves) {
        assert(piece != Piece::None);

        for (int i = 0; i < NODES; i++) {
            if (position[i] == piece) {
                for (int j = 0; j < NODES; j++) {
                    if (position[j] == Piece::None) {
                        make_move_move(piece, i, j);

                        if (is_mill(piece, j)) {
                            const auto opponent = opponent_piece(piece);

                            for (int k = 0; k < NODES; k++) {
                                if (position[k] == opponent) {
                                    if (is_mill(opponent, k) && !all_pieces_in_mills(opponent)) {
                                        continue;
                                    }

                                    moves.push_back(create_move_take(piece, i, j, k));
                                }
                            }
                        } else {
                            moves.push_back(create_move(piece, i, j));
                        }

                        unmake_move_move(piece, i, j);
                    }
                }
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
    }

    void SearchCtx::make_place_move(Piece piece, int node_index) {
        position[node_index] = piece;
    }

    void SearchCtx::unmake_place_move(int node_index) {
        position[node_index] = Piece::None;
    }

    void SearchCtx::make_move_move(Piece piece, int node_source_index, int node_destination_index) {
        position[node_destination_index] = piece;
        position[node_source_index] = Piece::None;
    }

    void SearchCtx::unmake_move_move(Piece piece, int node_source_index, int node_destination_index) {
        position[node_destination_index] = Piece::None;
        position[node_source_index] = piece;
    }

    int SearchCtx::evaluate_position() {  // TODO also evaluate piece positions
        positions_evaluated++;

        int evaluation = 0;

        const unsigned int white_material = calculate_material(Piece::White);
        const unsigned int black_material = calculate_material(Piece::Black);

        evaluation += white_material * parameters.PIECE;
        evaluation -= black_material * parameters.PIECE;

        const unsigned int white_freedom = calculate_freedom(Piece::White);
        const unsigned int black_freedom = calculate_freedom(Piece::Black);

        evaluation += white_freedom * parameters.FREEDOM;
        evaluation -= black_freedom * parameters.FREEDOM;

        return evaluation;
    }

    unsigned int SearchCtx::calculate_material(Piece piece) {
        assert(piece != Piece::None);

        unsigned int piece_count = 0;

        for (int i = 0; i < NODES; i++) {
            piece_count += position[i] == piece;
        }

        return piece_count;
    }

    unsigned int SearchCtx::calculate_freedom(Piece piece) {
        assert(piece != Piece::None);

        unsigned int total_free_positions = 0;

        for (int i = 0; i < NODES; i++) {
            if (position[i] == piece) {
                total_free_positions += calculate_piece_freedom(i);
            }
        }

        return total_free_positions;
    }

    unsigned int SearchCtx::calculate_piece_freedom(int index) {
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

    bool SearchCtx::all_pieces_in_mills(Piece piece) {
        for (int i = 0; i < NODES; i++) {
            if (position[i] == piece) {
                if (!is_mill(piece, i)) {
                    return false;
                }
            }
        }

        return true;
    }

#define IS_FREE_CHECK(const_index) \
    if (position[const_index] == Piece::None) { \
        result[pos++] = (const_index); \
    }

    std::array<int, 5> SearchCtx::neighbor_free_positions(int index) {
        std::array<int, 5> result = { -1, -1, -1, -1, -1 };
        int pos = 0;

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
                IS_FREE_CHECK(4)
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

    bool SearchCtx::is_mill(Piece piece, int index) {
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

    bool SearchCtx::is_game_over() {
        const unsigned int total_white_pieces = total_number_of_pieces(Piece::White);  // TODO constexpr
        const unsigned int total_black_pieces = total_number_of_pieces(Piece::Black);

        if (total_white_pieces < 3) {
            return true;
        }

        if (total_black_pieces < 3) {
            return true;
        }

        if (calculate_freedom(Piece::White) == 0 && white_pieces_outside + black_pieces_outside == 0) {  // TODO
            return true;
        }

        if (calculate_freedom(Piece::Black) == 0 && white_pieces_outside + black_pieces_outside == 0) {
            return true;
        }

        return false;
    }

    unsigned int SearchCtx::number_of_pieces_outside(Piece piece) {
        assert(piece != Piece::None);

        switch (piece) {
            case Piece::White:
                return white_pieces_outside;
            case Piece::Black:
                return black_pieces_outside;
            default:
                break;
        }

        return 0;
    }

    unsigned int SearchCtx::total_number_of_pieces(Piece piece) {
        assert(piece != Piece::None);

        switch (piece) {
            case Piece::White:
                return white_pieces_on_board + white_pieces_outside;
            case Piece::Black:
                return black_pieces_on_board + black_pieces_outside;
            default:
                break;
        }

        return 0;
    }

    Move create_place(Piece piece, int node_index) {
        Move move;
        move.place.node_index = node_index;
        move.type = MoveType::Place;
        move.piece = piece;

        return move;
    }

    Move create_move(Piece piece, int node_source_index, int node_destination_index) {
        Move move;
        move.move.node_source_index = node_source_index;
        move.move.node_destination_index = node_destination_index;
        move.type = MoveType::Move;
        move.piece = piece;

        return move;
    }

    Move create_place_take(Piece piece, int node_index, int node_take_index) {
        Move move;
        move.place_take.node_index = node_index;
        move.place_take.node_take_index = node_take_index;
        move.type = MoveType::PlaceTake;
        move.piece = piece;

        return move;
    }

    Move create_move_take(Piece piece, int node_source_index, int node_destination_index, int node_take_index) {
        Move move;
        move.move_take.node_source_index = node_source_index;
        move.move_take.node_destination_index = node_destination_index;
        move.move_take.node_take_index = node_take_index;
        move.type = MoveType::MoveTake;
        move.piece = piece;

        return move;
    }
}
