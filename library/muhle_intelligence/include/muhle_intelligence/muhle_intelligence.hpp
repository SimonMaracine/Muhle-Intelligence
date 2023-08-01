#pragma once

#include <ostream>
#include <string_view>
#include <string>

#include <definitions.hpp>

namespace muhle {
    struct MuhleIntelligence {
        virtual ~MuhleIntelligence() = default;

        virtual void initialize() = 0;
        virtual void search(const Position& position, Player player, Result& result) = 0;
        virtual void join_thread() = 0;
        virtual void set_parameter(std::string_view parameter, int value) = 0;
    };
}
