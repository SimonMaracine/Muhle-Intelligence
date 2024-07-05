cmake_minimum_required(VERSION 3.20)

add_executable(muhle_player
    "src/main.cpp"
    "src/muhle_player.cpp"
    "src/muhle_player.hpp"
)

target_link_libraries(muhle_player PRIVATE
    gui_base
    common
)

enable_warnings(muhle_player)
set_cpp_standard(muhle_player)

if(MSVC)
    target_compile_options(muhle_player PRIVATE "/utf-8")
endif()
