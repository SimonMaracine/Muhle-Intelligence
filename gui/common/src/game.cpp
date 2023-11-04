#include <array>
#include <vector>
#include <functional>
#include <string_view>
#include <regex>
#include <cstddef>

#include <gui_base/gui_base.hpp>
#include <muhle_intelligence/definitions.hpp>

#include "common/game.hpp"

bool ThreefoldRepetition::check_position(const std::array<::Node, 24>& board, Player turn) {
    Position current_position;

    current_position.turn = turn;

    for (std::size_t i = 0; i < board.size(); i++) {
        switch (board[i].piece) {
            case Piece::None:
                current_position.board[i] = Node::Empty;
                break;
            case Piece::White:
                current_position.board[i] = Node::White;
                break;
            case Piece::Black:
                current_position.board[i] = Node::Black;
                break;
        }
    }

    unsigned int repetition = 1;

    for (const Position& position : positions) {
        if (position == current_position) {
            repetition++;

            if (repetition == 3) {
                return true;
            }
        }
    }

    positions.push_back(current_position);

    return false;
}

void ThreefoldRepetition::clear_repetition() {
    positions.clear();
}

void MoveLogging::log_move(const Move& move, Player player) {
    muhle::Move muhle_move;

    switch (move.type) {
        case MoveType::Place:
            muhle_move.type = muhle::MoveType::Place;
            muhle_move.place.place_index = static_cast<muhle::Idx>(move.place.place_index);
            break;
        case MoveType::PlaceTake:
            muhle_move.type = muhle::MoveType::PlaceTake;
            muhle_move.place_take.place_index = static_cast<muhle::Idx>(move.place_take.place_index);
            muhle_move.place_take.take_index = static_cast<muhle::Idx>(move.place_take.take_index);
            break;
        case MoveType::Move:
            muhle_move.type = muhle::MoveType::Move;
            muhle_move.move.source_index = static_cast<muhle::Idx>(move.move.source_index);
            muhle_move.move.destination_index = static_cast<muhle::Idx>(move.move.destination_index);
            break;
        case MoveType::MoveTake:
            muhle_move.type = muhle::MoveType::MoveTake;
            muhle_move.move_take.source_index = static_cast<muhle::Idx>(move.move_take.source_index);
            muhle_move.move_take.destination_index = static_cast<muhle::Idx>(move.move_take.destination_index);
            muhle_move.move_take.take_index = static_cast<muhle::Idx>(move.move_take.take_index);
            break;
    }

    muhle::Player muhle_player = player == Player::White ? muhle::Player::White : muhle::Player::Black;

    move_history.push_back(std::make_pair(muhle_move, muhle_player));
}

void MoveLogging::clear_history() {
    move_history.clear();
}

bool is_valid_smn(std::string_view string) {
    const std::regex pattern {"[0wb]{24} (w|b) [0-9]{1,10} [0-9]{1,10}"};

    return std::regex_match(string.cbegin(), string.cend(), pattern);
}
