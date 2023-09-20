cmake_minimum_required(VERSION 3.20)

add_executable(test_move_generation_speed
    "main.cpp"
)

target_link_libraries(test_move_generation_speed PRIVATE muhle_intelligence GTest::gtest_main)

target_compile_features(test_move_generation_speed PRIVATE cxx_std_17)
set_target_properties(test_move_generation_speed PROPERTIES CXX_EXTENSIONS OFF)
