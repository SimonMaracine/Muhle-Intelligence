#pragma once

#include <string>
#include <ostream>

#include <muhle_intelligence/definitions.hpp>

namespace muhle {
    std::string move_to_string(const Move& move, Player player);
    void print_result_statistics(const Result& result, std::ostream& stream);
    void print_result_statistics(const Result& result, std::string& string);
}
