#pragma once

#include <array>
#include <unordered_map>
#include <string>
#include <cstddef>

#include <definitions.hpp>

#include "muhle_intelligence/internal/array.hpp"
#include "muhle_intelligence/muhle_intelligence.hpp"

namespace muhle {
    // For every invocation of the AI algorithm, create a new search context object
    class StandardGame {
    public:
        void setup(std::unordered_map<std::string, int>& parameters);
        void figure_out_position(const Position& position);
        void search(Player player, Result& result);
    private:
        // Basic thinking concludes that there cannot be more than 58 moves in a ply
        static constexpr size_t MAX_MOVES = 58;

        Eval minimax(unsigned int depth, unsigned int plies_from_root, Eval alpha, Eval beta, Player player);
        unsigned int test_moves(Player player, unsigned int depth);

        Move random_move(Piece piece);
        void get_all_moves(Piece piece, Array<Move, MAX_MOVES>& moves);
        void get_moves_phase1(Piece piece, Array<Move, MAX_MOVES>& moves);
        void get_moves_phase2(Piece piece, Array<Move, MAX_MOVES>& moves);
        void get_moves_phase3(Piece piece, Array<Move, MAX_MOVES>& moves);

        void make_move(const Move& move);
        void unmake_move(const Move& move);

        void make_place_move(Piece piece, Idx node_index);
        void unmake_place_move(Idx node_index);
        void make_move_move(Piece piece, Idx node_source_index, Idx node_destination_index);
        void unmake_move_move(Piece piece, Idx node_source_index, Idx node_destination_index);

        Eval evaluate_position(Eval evaluation_game_over);
        unsigned int calculate_material(Piece piece);
        Eval calculate_material_both();
        void calculate_freedom_both(unsigned int& white, unsigned int& black);
        unsigned int calculate_piece_freedom(Idx index);
        Eval calculate_freedom_both();

        bool all_pieces_in_mills(Piece piece);
        std::array<Idx, 5> neighbor_free_positions(Idx index);
        Piece opponent_piece(Piece type);
        bool is_mill(Piece piece, Idx index);
        bool is_game_over(Eval& evaluation_game_over);
        unsigned int pieces_on_board(Piece piece);

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

    namespace moves {
        Move create_place(Piece piece, Idx node_index) noexcept;
        Move create_move(Piece piece, Idx node_source_index, Idx node_destination_index) noexcept;
        Move create_place_take(Piece piece, Idx node_index, Idx node_take_index) noexcept;
        Move create_move_take(Piece piece, Idx node_source_index, Idx node_destination_index, Idx node_take_index) noexcept;
    }
}
