#pragma once

#include <array>

#include <muhle_intelligence/definitions.hpp>

unsigned int get_total_move_count(muhle::SearchCtx& ctx, muhle::Player player, unsigned int depth);
std::array<muhle::Piece, muhle::NODES> make_up_position(const std::array<int, muhle::NODES>& numbers);
