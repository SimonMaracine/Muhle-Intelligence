#pragma once

#include <array>
#include <vector>
#include <string_view>

#include <gui_base/gui_base.hpp>

namespace board {
    using Idx = int;

    inline constexpr Idx NULL_INDEX {-1};

    enum class Player {
        White,
        Black
    };

    enum class MoveType {
        Place,
        PlaceTake,
        Move,
        MoveTake
    };

    enum class Node {
        Empty = -1,
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

    class MuhleBoard {
    public:
        void update(bool user_input = true);
        void reset(std::string_view position_string = "");

        void place(Idx place_index);
        void place_take(Idx place_index, Idx take_index);
        void move(Idx source_index, Idx destination_index);
        void move_take(Idx source_index, Idx destination_index, Idx take_index);
    private:
        void update_user_input();
        Idx get_index(ImVec2 position);
        static bool point_in_circle(ImVec2 point, ImVec2 circle, float radius);

        Board board {};
        Player turn {Player::White};
        GameOver game_over {GameOver::None};
        unsigned int plies {};
        unsigned int plies_without_advancement {};
        // TODO repetition history

        // GUI data
        float board_unit {};
        ImVec2 board_offset {};
        std::array<ImVec2, 24> positions {};
        Idx selected_node_index {NULL_INDEX};
        std::vector<Move> legal_moves;

        static constexpr float NODE_RADIUS {2.2f};
    };
}
