cmake_minimum_required(VERSION 3.20)

add_library(common STATIC
    "include/common/board.hpp"
    "include/common/game.hpp"
    "src/board.cpp"
    "src/game.cpp"
)

target_include_directories(common PUBLIC "include")

target_link_libraries(common PRIVATE gui_base)

set_cpp_standard(common)
enable_warnings(common)
