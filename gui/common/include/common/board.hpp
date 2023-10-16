#pragma once

#include <array>
#include <vector>

#include <gui_base/gui_base.hpp>

#include "game.hpp"

class MuhleBoard {
public:
    void update();
private:
    void update_nodes();
    void draw_pieces(ImDrawList* draw_list);
    void update_input();
    void load_font();
    Idx get_index(ImVec2 position);
    void try_place_piece(const Move& move, Idx index);
    std::vector<Move> generate_moves();
    void get_moves_phase1(Piece piece, std::vector<Move>& moves);
    void get_moves_phase2(Piece piece, std::vector<Move>& moves);
    void get_moves_phase3(Piece piece, std::vector<Move>& moves);
    unsigned int pieces_on_board(Piece piece);
    Player opponent(Player player);
    Piece opponent_piece(Piece type);
    void make_place_move(Piece piece, Idx node_index);
    void unmake_place_move(Idx node_index);
    void make_move_move(Piece piece, Idx node_source_index, Idx node_destination_index);
    void unmake_move_move(Piece piece, Idx node_source_index, Idx node_destination_index);
    Move create_place(Idx node_index);
    Move create_move(Idx node_source_index, Idx node_destination_index);
    Move create_place_take(Idx node_index, Idx node_take_index);
    Move create_move_take(Idx node_source_index, Idx node_destination_index, Idx node_take_index);
    bool is_mill(Piece piece, Idx index);
    std::vector<Idx> neighbor_free_positions(Idx index);
    bool all_pieces_in_mills(Piece piece);
    static bool point_in_node(ImVec2 position, const Node& node);

    ImFont* label_font = nullptr;
    float board_unit = 0.0f;
    ImVec2 board_offset;

    std::array<Node, 24> board {};
    Player turn = Player::White;
    unsigned int plies = 0;
    unsigned int white_pieces_on_board = 0;
    unsigned int white_pieces_outside = 9;
    unsigned int black_pieces_on_board = 0;
    unsigned int black_pieces_outside = 9;

    int selected_piece_index = INVALID_INDEX;
    std::array<bool, 2> can_jump = { false, false };
    bool must_take_piece = false;
    unsigned int plies_without_mills = 0;

    ThreefoldRepetition repetition;
    MoveLogging log;
};
