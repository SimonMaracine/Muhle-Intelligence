cmake_minimum_required(VERSION 3.20)

option(MUHLE_SHARED_LIBRARY "Build the AI library as static or dynamic" OFF)
option(MUHLE_BUILD_GUI "Build the GUI components" ON)

project("Muhle-Intelligence" LANGUAGES C CXX)

add_subdirectory(library)

if(MUHLE_BUILD_GUI)
    add_subdirectory(gui)
endif()

message(STATUS "Muhle: Building shared library: ${MUHLE_SHARED_LIBRARY}")
message(STATUS "Muhle: Building GUI: ${MUHLE_BUILD_GUI}")
