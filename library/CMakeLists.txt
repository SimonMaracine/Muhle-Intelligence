cmake_minimum_required(VERSION 3.20)

set(SOURCES
    "include/muhle_intelligence/internal/implementation.hpp"
    "include/muhle_intelligence/muhle_intelligence.hpp"
    "src/internal/implementation.cpp"
    "src/api.cpp"
    "src/muhle_intelligence.cpp"
)

if(MUHLE_SHARED_LIBRARY)
    add_library(muhle_intelligence SHARED ${SOURCES})
else()
    add_library(muhle_intelligence STATIC ${SOURCES})
endif()

target_include_directories(muhle_intelligence PUBLIC "include")

target_compile_features(muhle_intelligence PRIVATE cxx_std_17)
set_target_properties(muhle_intelligence PROPERTIES CXX_EXTENSIONS OFF)
