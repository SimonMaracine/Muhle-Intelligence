cmake_minimum_required(VERSION 3.20)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Variable used to control the build type")

option(MUHLE_SHARED_LIBRARY "Build the AI library as static or as dynamic" OFF)
option(MUHLE_BUILD_GUI "Build the GUI components" OFF)
option(MUHLE_BUILD_TESTS "Build tests" OFF)

project("Muhle-Intelligence" LANGUAGES CXX)

function(enable_warnings target)
    if(UNIX)
        target_compile_options(${target} PRIVATE "-Wall" "-Wextra" "-Wpedantic")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${target} PRIVATE "/W3")
    else()
        message(WARNING "Compiler is not MSVC or platform is not supported; warnings are not enabled")
    endif()
endfunction()

add_subdirectory(library)

if(MUHLE_BUILD_GUI)
    add_subdirectory(gui)
endif()

if(MUHLE_BUILD_TESTS)
    add_subdirectory(tests)
endif()

message(STATUS "Muhle: Building shared library: ${MUHLE_SHARED_LIBRARY}")
message(STATUS "Muhle: Building GUI: ${MUHLE_BUILD_GUI}")
message(STATUS "Muhle: Building tests: ${MUHLE_BUILD_TESTS}")
message(STATUS "Muhle: Build type: ${CMAKE_BUILD_TYPE}")
