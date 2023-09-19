#include <array>

#include <gtest/gtest.h>

#include <muhle_intelligence/internal/search_context.hpp>
#include <muhle_intelligence/internal/moves.hpp>

#include "helpers.hpp"

TEST(MovesTest, TotalMoveCountBeginning) {
    muhle::SearchCtx ctx;
    ctx.position = {};
    ctx.white_pieces_on_board = 0;
    ctx.black_pieces_on_board = 0;
    ctx.plies = 0;

    ASSERT_EQ(get_total_move_count(ctx, muhle::Player::White, 1), 24);
    ASSERT_EQ(get_total_move_count(ctx, muhle::Player::White, 2), 24 * 23);
    ASSERT_EQ(get_total_move_count(ctx, muhle::Player::White, 3), 24 * 23 * 22);
    ASSERT_EQ(get_total_move_count(ctx, muhle::Player::White, 4), 24 * 23 * 22 * 21);
    ASSERT_EQ(get_total_move_count(ctx, muhle::Player::White, 5), 5'140'800);
}

TEST(MovesTest, MakeMove) {
    muhle::SearchCtx ctx;
    ctx.position = {};
    ctx.white_pieces_on_board = 0;
    ctx.black_pieces_on_board = 0;
    ctx.plies = 0;

    {
        muhle::Move move;
        move.type = muhle::MoveType::Place;
        move.place.node_index = 5;

        muhle::make_move(ctx, move, muhle::Piece::White);
    }

    ASSERT_EQ(ctx.position, make_up_position({0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));

    {
        muhle::Move move;
        move.type = muhle::MoveType::Place;
        move.place.node_index = 4;

        muhle::make_move(ctx, move, muhle::Piece::Black);
    }

    ASSERT_EQ(ctx.position, make_up_position({0, 0, 0, 0, -1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
}
