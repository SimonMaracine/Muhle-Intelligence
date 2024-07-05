cmake_minimum_required(VERSION 3.20)

project("Muhle-Intelligence" LANGUAGES CXX)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Variable used to control the build type")

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

set(GUI_BASE_INCLUDE_FILE_DIALOG ON)

add_subdirectory(extern/tiny-gui-base)

add_subdirectory(common)
add_subdirectory(muhle_player)
add_subdirectory(muhle_tester)

message(STATUS "Muhle: Build type: ${CMAKE_BUILD_TYPE}")
