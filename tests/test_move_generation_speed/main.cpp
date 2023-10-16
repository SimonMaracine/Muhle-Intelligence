#include <chrono>
#include <cstddef>
#include <iostream>

#include <muhle_intelligence/internal/search_context.hpp>
#include <muhle_intelligence/internal/moves.hpp>

void move_generation(muhle::SearchCtx& ctx, muhle::Player player, unsigned int depth) {
    if (depth == 0) {
        return;
    }

    if (player == muhle::Player::White) {
        muhle::Array<muhle::Move, muhle::MAX_MOVES> moves;
        muhle::generate_moves(ctx, muhle::Piece::White, moves);

        for (const muhle::Move& move : moves) {
            muhle::make_move(ctx, move, muhle::Piece::White);
            move_generation(ctx, muhle::Player::Black, depth - 1);
            muhle::unmake_move(ctx, move, muhle::Piece::White);
        }
    } else {
        muhle::Array<muhle::Move, muhle::MAX_MOVES> moves;
        muhle::generate_moves(ctx, muhle::Piece::Black, moves);

        for (const muhle::Move& move : moves) {
            muhle::make_move(ctx, move, muhle::Piece::Black);
            move_generation(ctx, muhle::Player::White, depth - 1);
            muhle::unmake_move(ctx, move, muhle::Piece::Black);
        }
    }
}

int main() {
    std::chrono::duration<double> total;
    static constexpr std::size_t ITERATIONS = 16;

    muhle::SearchCtx ctx;
    ctx.board = {};
    ctx.white_pieces_on_board = 0;
    ctx.black_pieces_on_board = 0;
    ctx.plies = 0;

    for (std::size_t i = 0; i < ITERATIONS; i++) {
        const auto start = std::chrono::high_resolution_clock::now();

        move_generation(ctx, muhle::Player::White, 5);

        const auto stop = std::chrono::high_resolution_clock::now();

        total += stop - start;
    }

    const float result = (
        static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(total).count()) / static_cast<float>(ITERATIONS)
    );

    std::cout
        << "Average time: "
        << result
        << " ms, iterations: "
        << ITERATIONS
        << '\n';
}
