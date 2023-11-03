cmake_minimum_required(VERSION 3.20)

add_executable(test_library
    "helpers.cpp"
    "helpers.hpp"
    "main.cpp"
)

target_link_libraries(test_library PRIVATE muhle_intelligence GTest::gtest_main)

set_cpp_standard(test_library)
enable_warnings(test_library)
