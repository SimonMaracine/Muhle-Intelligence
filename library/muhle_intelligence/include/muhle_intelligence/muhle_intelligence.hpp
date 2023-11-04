#pragma once

#include <string>
#include <vector>

#include <muhle_intelligence/definitions.hpp>

namespace muhle {
    struct MuhleIntelligence {
        virtual ~MuhleIntelligence() = default;

        virtual void initialize() = 0;
        virtual void new_game(const SmnPosition& position = {}, const std::vector<Move>& moves = {}) = 0;
        virtual void go(Result& result, bool play_move = true) = 0;
        virtual void move(const Move& move) = 0;
        virtual void join_thread() = 0;
        virtual void set_parameter(const std::string& parameter, int value) = 0;
    };
}
