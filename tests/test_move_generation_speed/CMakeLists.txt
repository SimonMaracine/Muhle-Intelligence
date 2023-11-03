cmake_minimum_required(VERSION 3.20)

add_executable(test_move_generation_speed
    "main.cpp"
)

target_link_libraries(test_move_generation_speed PRIVATE muhle_intelligence GTest::gtest_main)

set_cpp_standard(test_move_generation_speed)
enable_warnings(test_move_generation_speed)
