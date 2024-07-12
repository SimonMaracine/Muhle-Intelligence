cmake_minimum_required(VERSION 3.20)

project("Muhle-Intelligence-Tests" LANGUAGES CXX)

set(INSTALL_GTEST OFF)
set(BUILD_GMOCK OFF)

add_subdirectory(extern/googletest)

add_custom_target(copy_include ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    "${CMAKE_CURRENT_BINARY_DIR}/include"
)

add_executable(muhle_intelligence_test "main.cpp")

add_custom_command(TARGET muhle_intelligence_test PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/../muhle_intelligence/target/debug/libmuhle_intelligence.so"
    "${CMAKE_CURRENT_BINARY_DIR}"
)

target_include_directories(muhle_intelligence_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/include")

target_link_directories(muhle_intelligence_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

target_link_libraries(muhle_intelligence_test PRIVATE GTest::gtest_main)
target_link_libraries(muhle_intelligence_test PRIVATE muhle_intelligence)

if(UNIX)
    target_compile_options(muhle_intelligence_test PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Wconversion")
elseif(MSVC)
    target_compile_options(muhle_intelligence_test PRIVATE "/W4")
else()
    message(WARNING "Warnings are not enabled")
endif()

target_compile_features(muhle_intelligence_test PRIVATE cxx_std_17)
set_target_properties(muhle_intelligence_test PROPERTIES CXX_EXTENSIONS OFF)
