cmake_minimum_required(VERSION 3.20)

add_library(common STATIC
    "include/common/board.hpp"
    "include/common/game.hpp"
    "src/board.cpp"
    "src/game.cpp"
)

target_link_libraries(common PRIVATE
    gui_base
    muhle_intelligence_api
    muhle_intelligence_miscellaneous
)

target_include_directories(common PUBLIC "include")

target_compile_features(common PRIVATE cxx_std_20)
set_target_properties(common PROPERTIES CXX_EXTENSIONS OFF)

enable_warnings(common)

if(UNIX)
    target_compile_options(common PRIVATE "-Wno-parentheses")
endif()
