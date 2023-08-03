#pragma once

#include <string>
#include <iostream>

#include <definitions.hpp>

namespace muhle {
    std::string move_to_string(const Move& move);
    void print_result(const Result& result, std::ostream& stream = std::cout);
    void print_result(const Result& result, std::string& string);
}
