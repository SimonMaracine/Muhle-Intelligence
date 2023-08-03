cmake_minimum_required(VERSION 3.20)

add_executable(muhle_tester
    "src/main.cpp"
    "src/muhle_tester.hpp"
    "src/muhle_tester.cpp"
    "src/game.hpp"
    "src/game.cpp"
)

target_link_libraries(muhle_tester PRIVATE
    gui_base
    glm::glm
    just_dl
    muhle_intelligence_api
    muhle_intelligence_miscellaneous
)

target_compile_features(muhle_tester PRIVATE cxx_std_17)
set_target_properties(muhle_tester PROPERTIES CXX_EXTENSIONS OFF)

enable_warnings(muhle_tester)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(muhle_tester PRIVATE "/utf-8")
endif()
