#pragma once

#include <string>
#include <iostream>

#include <muhle_intelligence/definitions.hpp>

namespace muhle {
    std::string move_to_string(const Move& move, Player player);
    void print_result_statistics(const Result& result, std::ostream& stream = std::cout);
    void print_result_statistics(const Result& result, std::string& string);
}
