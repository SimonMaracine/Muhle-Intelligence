cmake_minimum_required(VERSION 3.20)

add_library(common STATIC
    "include/common/board.hpp"
    "include/common/subprocess.hpp"
    "src/board.cpp"
    "src/subprocess.cpp"
)

target_include_directories(common PUBLIC "include")

target_link_libraries(common PRIVATE gui_base)

enable_warnings(common)
set_cpp_standard(common)
