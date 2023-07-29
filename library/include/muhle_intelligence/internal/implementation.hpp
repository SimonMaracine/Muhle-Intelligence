#pragma once

#include <array>
#include <thread>
#include <vector>
#include <unordered_map>
#include <string>
#include <string_view>

#include "muhle_intelligence/muhle_intelligence.hpp"

namespace muhle {
    struct MuhleImpl : public MuhleIntelligence {
        virtual void initialize() override;
        virtual void search(const Position& position, Player player, Result& result) override;
        virtual void join_thread() override;
        virtual void set_parameter(std::string_view parameter, int value) override;

        std::thread thread;
        std::unordered_map<std::string, int> parameters;
    };

    class SearchCtx {
    public:
        void setup(const std::unordered_map<std::string, int>& parameters);
        void search(Position position, Player player, Result& result);
    private:
        int minimax(unsigned int depth, unsigned int turns_from_root, int alpha, int beta, Player player);

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

        int evaluate_position();
        unsigned int calculate_material(Piece piece);
        unsigned int calculate_freedom(Piece piece);
        unsigned int calculate_piece_freedom(int index);

        bool all_pieces_in_mills(Piece piece);
        std::array<int, 5> neighbor_free_positions(int index);
        Piece opponent_piece(Piece type);
        bool is_mill(Piece piece, int index);
        bool is_game_over();
        unsigned int number_of_pieces_outside(Piece piece);
        unsigned int total_number_of_pieces(Piece piece);

        std::array<Piece, NODES> position {};
        unsigned int plies_until_this_position = 0;
        unsigned int white_pieces_on_board = 0;
        unsigned int white_pieces_outside = 0;
        unsigned int black_pieces_on_board = 0;
        unsigned int black_pieces_outside = 0;

        Move best_move {};
        int evaluation = 0;
        unsigned int positions_evaluated = 0;

        struct {
            int PIECE {};
            int FREEDOM {};
            int DEPTH {};
        } parameters;
    };
}
