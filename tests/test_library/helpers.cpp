#include <array>

#include <muhle_intelligence/definitions.hpp>
#include <muhle_intelligence/internal/array.hpp>
#include <muhle_intelligence/internal/moves.hpp>
#include <muhle_intelligence/internal/search_context.hpp>

#include "helpers.hpp"

unsigned int get_total_move_count(muhle::SearchCtx& ctx, muhle::Player player, unsigned int depth) {
    if (depth == 0) {
        return 1;
    }

    unsigned int move_count = 0;

    if (player == muhle::Player::White) {
        muhle::Array<muhle::Move, muhle::MAX_MOVES> moves;
        muhle::generate_moves(ctx, muhle::Piece::White, moves);

        for (const muhle::Move& move : moves) {
            muhle::make_move(ctx, move, muhle::Piece::White);
            move_count += get_total_move_count(ctx, muhle::Player::Black, depth - 1);
            muhle::unmake_move(ctx, move, muhle::Piece::White);
        }
    } else {
        muhle::Array<muhle::Move, muhle::MAX_MOVES> moves;
        muhle::generate_moves(ctx, muhle::Piece::Black, moves);

        for (const muhle::Move& move : moves) {
            muhle::make_move(ctx, move, muhle::Piece::Black);
            move_count += get_total_move_count(ctx, muhle::Player::White, depth - 1);
            muhle::unmake_move(ctx, move, muhle::Piece::Black);
        }
    }

    return move_count;
}

std::array<muhle::Piece, muhle::NODES> make_up_board(const std::array<int, muhle::NODES>& numbers) {
    std::array<muhle::Piece, muhle::NODES> result;

    for (muhle::IterIdx i = 0; i < muhle::NODES; i++) {
        switch (numbers[i]) {
            case -1:
                result[i] = muhle::Piece::Black;
                break;
            case 0:
                result[i] = muhle::Piece::None;
                break;
            case 1:
                result[i] = muhle::Piece::White;
                break;
        }
    }

    return result;
}
