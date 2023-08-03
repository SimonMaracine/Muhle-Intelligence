#pragma once

#include <array>
#include <thread>
#include <vector>
#include <unordered_map>
#include <string>
#include <string_view>

#include <definitions.hpp>

#include "muhle_intelligence/muhle_intelligence.hpp"

namespace muhle {
    inline constexpr size_t MAX_PLY_MOVES = 57;

    struct MuhleImpl : public MuhleIntelligence {
        virtual void initialize() override;
        virtual void search(const Position& position, Player player, Result& result) override;
        virtual void join_thread() override;
        virtual void set_parameter(std::string_view parameter, int value) override;

        std::thread thread;
        std::unordered_map<std::string, int> parameters;
    };

    // For every invocation of the AI algorithm, create a new search context object
    class SearchCtx {
    public:
        void setup(const std::unordered_map<std::string, int>& parameters);
        void figure_out_position(const Position& position);
        void search(Player player, Result& result);
    private:
        int minimax(unsigned int depth, unsigned int turns_from_root, int alpha, int beta, Player player);
        unsigned int test_moves(Player player, unsigned int depth);

        Move random_move(Piece piece);
        std::vector<Move> get_all_moves(Piece piece);
        void get_moves_phase1(Piece piece, std::vector<Move>& moves);
        void get_moves_phase2(Piece piece, std::vector<Move>& moves);
        void get_moves_phase3(Piece piece, std::vector<Move>& moves);

        void make_move(const Move& move);
        void unmake_move(const Move& move);

        void make_place_move(Piece piece, int node_index);
        void unmake_place_move(int node_index);
        void make_move_move(Piece piece, int node_source_index, int node_destination_index);
        void unmake_move_move(Piece piece, int node_source_index, int node_destination_index);

        int evaluate_position(int game_over_evaluation);
        unsigned int calculate_material(Piece piece);
        unsigned int calculate_freedom(Piece piece);
        unsigned int calculate_piece_freedom(int index);

        bool all_pieces_in_mills(Piece piece);
        std::array<int, 5> neighbor_free_positions(int index);
        Piece opponent_piece(Piece type);
        bool is_mill(Piece piece, int index);
        bool is_game_over(int& game_over_evaluation);
        unsigned int pieces_on_board(Piece piece);

        // TODO cache stuff
        std::array<Piece, NODES> position {};
        unsigned int white_pieces_on_board {};
        unsigned int black_pieces_on_board {};
        unsigned int plies {};

        Move best_move {};
        unsigned int positions_evaluated = 0;

        struct {
            int PIECE {};
            int FREEDOM {};
            int END_GAME {};
            int DEPTH {};
        } parameters;
    };

    Move create_place(Piece piece, int node_index);
    Move create_move(Piece piece, int node_source_index, int node_destination_index);
    Move create_place_take(Piece piece, int node_index, int node_take_index);
    Move create_move_take(Piece piece, int node_source_index, int node_destination_index, int node_take_index);
}
