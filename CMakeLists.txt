cmake_minimum_required(VERSION 3.20)

option(MUHLE_SHARED_LIBRARY OFF "Build the library as static or dynamic")

project("Muhle-Intelligence" LANGUAGES CXX)

message(STATUS "Muhle: building shared library: ${MUHLE_SHARED_LIBRARY}")

add_subdirectory(library)
add_subdirectory(gui)
