#pragma once

#include <array>
#include <vector>

#include <gui_base/gui_base.hpp>

#include "game.hpp"

class MuhleBoard {
public:
    void update();
private:
    std::vector<Move> generate_moves();

    unsigned int white_pieces_on_board = 0;
    unsigned int white_pieces_outside = 9;
    unsigned int black_pieces_on_board = 0;
    unsigned int black_pieces_outside = 9;

    Player turn = Player::White;
    GamePhase phase = GamePhase::PlacePieces;
    Ending ending = Ending::None;

    unsigned int plies = 0;
    std::array<Node, 24> nodes {};
    int selected_piece_index = INVALID_INDEX;
    std::array<bool, 2> can_jump = { false, false };
    bool must_take_piece = false;
    unsigned int plies_without_mills = 0;

    ThreefoldRepetition repetition;
    MoveLogging log;
};
