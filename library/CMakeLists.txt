cmake_minimum_required(VERSION 3.20)

set(SOURCES
    "common.hpp"
    "common.cpp"
    "minimax_thread.hpp"
    "minimax_thread.cpp"
    "minimax.hpp"
    "minimax.cpp"
)

if(MUHLE_SHARED_LIBRARY)
    add_library(muhle_intelligence SHARED ${SOURCES})
else()
    add_library(muhle_intelligence STATIC ${SOURCES})
endif()

target_compile_features(muhle_intelligence PRIVATE cxx_std_17)
set_target_properties(muhle_intelligence PROPERTIES CXX_EXTENSIONS OFF)
