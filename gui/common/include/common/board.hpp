#pragma once

#include <array>
#include <vector>
#include <string_view>
#include <string>
#include <cstddef>
#include <utility>
#include <functional>

#include <gui_base/gui_base.hpp>
#include <muhle_intelligence/definitions.hpp>

#include "game.hpp"

class MuhleBoard {
public:
    using MoveCallback = std::function<void(const Move&)>;  // Called when user makes a move

    MuhleBoard() {
        reset();
    }

    void update();
    void reset();
    bool set_position(std::string_view smn_string);

    void place_piece(Idx place_index);
    void place_take_piece(Idx place_index, Idx take_index);
    void move_piece(Idx source_index, Idx destination_index);
    void move_take_piece(Idx source_index, Idx destination_index, Idx take_index);

    Player get_turn() const { return turn; }
    bool is_game_over() const { return game_over != GameOver::None; }
    void set_move_callback(const MoveCallback& move_callback) { this->move_callback = move_callback; }

    bool user_input = true;
private:
    void internals_window();
    void moves_window();
    void update_nodes();
    void draw_pieces(ImDrawList* draw_list);
    void update_input();
    void load_font();
    Idx get_index(ImVec2 position);
    bool select_piece(Idx index);
    void change_turn();
    void change_turn_after_take();
    void check_winner_material();
    void check_winner_blocking();
    void maybe_generate_moves();
    void try_place(const Move& move, Idx place_index);
    void try_place_take(const Move& move, Idx place_index, Idx take_index);
    void try_move(const Move& move, Idx source_index, Idx destination_index);
    void try_move_take(const Move& move, Idx source_index, Idx destination_index, Idx take_index);
    void do_place(Idx place_index);
    void do_place_take(Idx place_index, Idx take_index);
    void do_move(Idx source_index, Idx destination_index);
    void do_move_take(Idx source_index, Idx destination_index, Idx take_index);
    std::vector<Move> generate_moves();
    void get_moves_phase1(Piece piece, std::vector<Move>& moves);
    void get_moves_phase2(Piece piece, std::vector<Move>& moves);
    void get_moves_phase3(Piece piece, std::vector<Move>& moves);
    unsigned int pieces_on_board(Piece piece);
    Player opponent(Player player);
    Piece opponent_piece(Piece piece);
    void make_place_move(Piece piece, Idx place_index);
    void unmake_place_move(Idx place_index);
    void make_move_move(Piece piece, Idx source_index, Idx destination_index);
    void unmake_move_move(Piece piece, Idx source_index, Idx destination_index);
    Move create_place(Idx place_index);
    Move create_move(Idx source_index, Idx destination_index);
    Move create_place_take(Idx place_index, Idx take_index);
    Move create_move_take(Idx source_index, Idx destination_index, Idx take_index);
    bool is_mill(Piece piece, Idx index);
    std::vector<Idx> neighbor_free_positions(Idx index);
    bool all_pieces_in_mills(Piece piece);
    unsigned int count_pieces_on_board(Piece piece);
    static bool point_in_node(ImVec2 position, const Node& node, float radius);
    static std::pair<unsigned int, std::size_t> parse_integer(std::string_view string, std::size_t position);

    ImFont* label_font = nullptr;
    float board_unit = 0.0f;
    ImVec2 board_offset;
    std::vector<Move> legal_moves;
    MoveLogging log;
    GameOver game_over = GameOver::None;
    MoveCallback move_callback;  // Called before board state is changed

    std::array<Node, 24> board {};
    Player turn = Player::White;
    unsigned int plies = 0;
    unsigned int plies_without_mills = 0;
    ThreefoldRepetition repetition;

    Idx selected_piece_index = INVALID_INDEX;
    unsigned int white_pieces_on_board = 0;
    unsigned int black_pieces_on_board = 0;
    bool must_take_piece = false;

    static constexpr float NODE_RADIUS = 2.2f;
};
