cmake_minimum_required(VERSION 3.20)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Variable used to control the build type")

option(MUHLE_BUILD_GUI "Build the GUI components" OFF)
option(MUHLE_BUILD_TESTS "Build tests" OFF)

project("Muhle-Intelligence" LANGUAGES CXX)

function(enable_warnings target)
    if(UNIX)
        target_compile_options(${target} PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Wconversion")
    elseif(MSVC)
        target_compile_options(${target} PRIVATE "/W4")
    else()
        message(WARNING "Warnings are not enabled")
    endif()
endfunction()

function(set_cpp_standard target)
    target_compile_features(${target} PRIVATE cxx_std_17)
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
endfunction()

add_subdirectory(library)

if(MUHLE_BUILD_GUI)
    add_subdirectory(gui)
endif()

if(MUHLE_BUILD_TESTS)
    add_subdirectory(tests)
endif()

message(STATUS "Muhle: Building GUI: ${MUHLE_BUILD_GUI}")
message(STATUS "Muhle: Building tests: ${MUHLE_BUILD_TESTS}")
message(STATUS "Muhle: Build type: ${CMAKE_BUILD_TYPE}")
