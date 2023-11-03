cmake_minimum_required(VERSION 3.20)

add_executable(muhle_player
    "src/main.cpp"
    "src/muhle_player.hpp"
    "src/muhle_player.cpp"
)

target_link_libraries(muhle_player PRIVATE
    gui_base
    just_dl
    muhle_intelligence_api
    muhle_intelligence_miscellaneous
    common
)

set_cpp_standard(muhle_player)
enable_warnings(muhle_player)

if(UNIX)
    target_compile_options(muhle_player PRIVATE "-Wno-cast-function-type")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(muhle_player PRIVATE "/utf-8")
endif()
