#pragma once

#include <string_view>

#include <muhle_intelligence/definitions.hpp>

namespace muhle {
    enum class Game {
        StandardGame
    };

    struct MuhleIntelligence {
        virtual ~MuhleIntelligence() = default;

        virtual void initialize(Game game) = 0;
        virtual void search(const Position& position, Player player, Result& result) = 0;
        virtual void join_thread() = 0;
        virtual void set_parameter(std::string_view parameter, int value) = 0;
    };
}
