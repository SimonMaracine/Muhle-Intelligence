cmake_minimum_required(VERSION 3.20)

add_executable(muhle_tester
    "main.cpp"
    "muhle_tester.hpp"
    "muhle_tester.cpp"
    "game.hpp"
    "game.cpp"
)

target_link_libraries(muhle_tester PRIVATE
    gui_base
    glm::glm
    just_dl
    muhle_intelligence_api
)

target_compile_features(muhle_tester PRIVATE cxx_std_17)
set_target_properties(muhle_tester PROPERTIES CXX_EXTENSIONS OFF)
