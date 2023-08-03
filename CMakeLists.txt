cmake_minimum_required(VERSION 3.20)

option(MUHLE_SHARED_LIBRARY "Build the AI library as static or as dynamic" OFF)
option(MUHLE_BUILD_GUI "Build the GUI components" ON)

project("Muhle-Intelligence" LANGUAGES C CXX)

function(enable_warnings target)
    if(UNIX)
        target_compile_options(${target} PRIVATE "-Wall" "-Wextra" "-Wpedantic")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${target} PRIVATE "/W3")
    else()
        message(WARNING "Compiler is not MSVC or platform is not supported; warnings were not enabled")
    endif()
endfunction()

add_subdirectory(library)

if(MUHLE_BUILD_GUI)
    add_subdirectory(gui)
endif()

message(STATUS "Muhle: Building shared library: ${MUHLE_SHARED_LIBRARY}")
message(STATUS "Muhle: Building GUI: ${MUHLE_BUILD_GUI}")
message(STATUS "Muhle: Build type: ${CMAKE_BUILD_TYPE}")
