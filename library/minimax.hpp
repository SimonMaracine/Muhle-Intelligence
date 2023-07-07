#pragma once

#include <vector>
#include <array>
#include <atomic>

#include "common.hpp"

class MinimaxStandardGame : public MinimaxAlgorithm {
public:
    MinimaxStandardGame() = default;
    ~MinimaxStandardGame() = default;

    MinimaxStandardGame(const MinimaxStandardGame&) = default;
    MinimaxStandardGame& operator=(const MinimaxStandardGame&) = default;
    MinimaxStandardGame(MinimaxStandardGame&&) noexcept = default;
    MinimaxStandardGame& operator=(MinimaxStandardGame&&) noexcept = default;

    virtual void search(Position position, Piece piece, Move& result, std::atomic<bool>& running) override;

    struct {
        int PIECE = 7;
        int FREEDOM = 1;
        unsigned int DEPTH = 5;
    } parameters;
private:
    int minimax(unsigned int depth, unsigned int turns_from_root, int alpha, int beta, Piece type);
    void random_move(Piece piece);

    std::vector<Move> get_all_moves(Piece piece);
    void get_moves_phase1(Piece piece, std::vector<Move>& moves);
    void get_moves_phase2(Piece piece, std::vector<Move>& moves);
    void get_moves_phase3(Piece piece, std::vector<Move>& moves);

    void make_move(const Move& move);
    void unmake_move(const Move& move);

    void make_place_move(Piece piece, int place_node_index);
    void unmake_place_move(int place_node_index);
    void make_move_move(Piece piece, int move_source_node_index, int move_destination_node_index);
    void unmake_move_move(Piece piece, int move_source_node_index, int move_destination_node_index);

    int evaluate_position();
    unsigned int calculate_material(Piece piece);
    unsigned int calculate_freedom(Piece piece);
    unsigned int calculate_piece_freedom(int index);

    bool all_pieces_in_mills(Piece piece);
    std::array<int, 5> neighbor_free_positions(int index);
    Piece opponent_piece(Piece type);
    bool is_mill(Piece piece, int index);
    bool is_game_over();
    unsigned int number_of_pieces_outside(Piece type);
    unsigned int total_number_of_pieces(Piece type);

    Position position;
    Move best_move;
    int evaluation = 0;
    unsigned int positions_evaluated = 0;
};