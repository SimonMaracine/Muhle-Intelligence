# The CMake system in this project is only for testng purposes.
# You can either use it to build the tests or to link your client CMake target to the library version of the engine.

cmake_minimum_required(VERSION 3.20)

project("Muhle-Intelligence" LANGUAGES CXX)

option(MUHLE_TESTS "Enable library tests" OFF)
set(MUHLE_OUTPUT_DIR "" CACHE PATH "Output directory path desired by the user for the library to be copied to")

if(MUHLE_TESTS)
    set(MUHLE_OUTPUT_DIR "${CMAKE_BINARY_DIR}")
endif()

if(NOT MUHLE_OUTPUT_DIR)
    message(FATAL_ERROR "Library output directory not set. Do either `set(MUHLE_OUTPUT_DIR <value>)` or `-DMUHLE_OUTPUT_DIR=<value>`.")
endif()

add_custom_target(build_muhle_intelligence ALL
    COMMAND "cargo" "build" "--manifest-path" "${CMAKE_CURRENT_SOURCE_DIR}/muhle_intelligence/Cargo.toml"
)

add_library(muhle_intelligence_interface INTERFACE)
target_include_directories(muhle_intelligence_interface INTERFACE "${MUHLE_OUTPUT_DIR}/include")
target_link_directories(muhle_intelligence_interface INTERFACE "${MUHLE_OUTPUT_DIR}")
target_link_libraries(muhle_intelligence_interface INTERFACE muhle_intelligence)

add_custom_target(copy_include ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${MUHLE_OUTPUT_DIR}/include"
)

add_custom_target(copy_library ALL
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/muhle_intelligence/target/debug/libmuhle_intelligence.so"  # FIXME
    "${MUHLE_OUTPUT_DIR}/libmuhle_intelligence.so"
)

if(MUHLE_TESTS)
    add_subdirectory(tests)
endif()
