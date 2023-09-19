cmake_minimum_required(VERSION 3.20)

add_executable(test_library
    "helpers.cpp"
    "helpers.hpp"
    "main.cpp"
)

target_link_libraries(test_library PRIVATE muhle_intelligence GTest::gtest_main)

target_compile_features(test_library PRIVATE cxx_std_17)
set_target_properties(test_library PROPERTIES CXX_EXTENSIONS OFF)
