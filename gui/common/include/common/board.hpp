#pragma once

#include <array>
#include <vector>
#include <string_view>

#include <gui_base/gui_base.hpp>

namespace board {
    using Idx = int;

    inline constexpr Idx NULL_INDEX {-1};

    enum class Player {
        White = 1,
        Black = 2
    };

    enum class MoveType {
        Place,
        PlaceTake,
        Move,
        MoveTake
    };

    enum class Node {
        Empty,
        White,
        Black
    };

    enum class GameOver {
        None,
        WinnerWhite,
        WinnerBlack,
        TieBetweenBothPlayers
    };

    struct Move {
        union {
            struct {
                Idx place_index;
            } place;

            struct {
                Idx place_index;
                Idx take_index;
            } place_take;

            struct {
                Idx source_index;
                Idx destination_index;
            } move;

            struct {
                Idx source_index;
                Idx destination_index;
                Idx take_index;
            } move_take;
        };

        MoveType type {};
    };

    using Board = std::array<Node, 24>;

    struct Position {
        Board board {};
        Player turn {};

        bool operator==(const Position& other) const {
            return board == other.board && turn == other.turn;
        }
    };

    class MuhleBoard {
    public:
        MuhleBoard();

        void update(bool user_input = true);
        void reset(std::string_view position_string = "");
        void debug() const;

        void place(Idx place_index);
        void place_take(Idx place_index, Idx take_index);
        void move(Idx source_index, Idx destination_index);
        void move_take(Idx source_index, Idx destination_index, Idx take_index);
    private:
        void update_user_input();
        bool select_piece(Idx index);
        void try_place(Idx place_index);
        void try_place_take(Idx place_index, Idx take_index);
        void try_move(Idx source_index, Idx destination_index);
        void try_move_take(Idx source_index, Idx destination_index, Idx take_index);
        void finish_turn(bool advancement = true);
        void check_winner_material();
        void check_winner_blocking();
        void check_fifty_move_rule();
        void check_threefold_repetition(const Position& position);
        std::vector<Move> generate_moves() const;
        static void generate_moves_phase1(Player player, Board& board, std::vector<Move>& moves);
        static void generate_moves_phase2(Player player, Board& board, std::vector<Move>& moves);
        static void generate_moves_phase3(Player player, Board& board, std::vector<Move>& moves);
        static void make_place_move(Player player, Idx place_index, Board& board);
        static void unmake_place_move(Idx place_index, Board& board);
        static void make_move_move(Idx source_index, Idx destination_index, Board& board);
        static void unmake_move_move(Idx source_index, Idx destination_index, Board& board);
        static bool is_mill(Player player, Idx index, const Board& board);
        static bool all_pieces_in_mills(Player player, const Board& board);
        static std::vector<Idx> neighbor_free_positions(Idx index, const Board& board);
        static Move create_place(Idx place_index);
        static Move create_place_take(Idx place_index, Idx take_index);
        static Move create_move(Idx source_index, Idx destination_index);
        static Move create_move_take(Idx source_index, Idx destination_index, Idx take_index);
        Idx get_index(ImVec2 position) const;
        unsigned int count_pieces(Player player) const;
        static Player opponent(Player player);
        static bool point_in_circle(ImVec2 point, ImVec2 circle, float radius);

        Board board {};
        Player turn {Player::White};
        GameOver game_over {GameOver::None};
        unsigned int plies {};
        unsigned int plies_without_advancement {};
        std::vector<Position> positions;

        // GUI data
        float board_unit {};
        ImVec2 board_offset {};
        Idx user_stored_index1 {NULL_INDEX};
        Idx user_stored_index2 {NULL_INDEX};
        bool user_must_take_piece {false};
        std::vector<Move> legal_moves;

        static constexpr float NODE_RADIUS {2.2f};
    };
}
