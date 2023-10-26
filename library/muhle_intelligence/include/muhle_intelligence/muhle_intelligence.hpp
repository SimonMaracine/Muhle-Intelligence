#pragma once

#include <string_view>
#include <vector>

#include <muhle_intelligence/definitions.hpp>

namespace muhle {
    struct MuhleIntelligence {
        virtual ~MuhleIntelligence() = default;

        virtual void initialize() = 0;
        virtual void new_game() = 0;
        virtual void position(const SmnPosition& position, const std::vector<Move>& moves) = 0;
        virtual void search(Result& result) = 0;
        virtual void join_thread() = 0;
        virtual void set_parameter(std::string_view parameter, int value) = 0;
    };
}
