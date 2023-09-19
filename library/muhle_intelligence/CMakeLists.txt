cmake_minimum_required(VERSION 3.20)

set(SOURCES
    "include/muhle_intelligence/internal/array.hpp"
    "include/muhle_intelligence/internal/evaluation.hpp"
    "include/muhle_intelligence/internal/library.hpp"
    "include/muhle_intelligence/internal/moves.hpp"
    "include/muhle_intelligence/internal/search_context.hpp"
    "include/muhle_intelligence/internal/search.hpp"
    "include/muhle_intelligence/internal/various.hpp"
    "include/muhle_intelligence/muhle_intelligence.hpp"
    "src/api.cpp"
    "src/evaluation.cpp"
    "src/library.cpp"
    "src/moves.cpp"
    "src/search.cpp"
    "src/various.cpp"
)

if(MUHLE_SHARED_LIBRARY)
    add_library(muhle_intelligence SHARED ${SOURCES})
    add_library(muhle_intelligence_api INTERFACE)
else()
    add_library(muhle_intelligence STATIC ${SOURCES})
endif()

target_link_libraries(muhle_intelligence PUBLIC muhle_intelligence_definitions)

target_include_directories(muhle_intelligence PUBLIC "include")

if(MUHLE_SHARED_LIBRARY)
    target_include_directories(muhle_intelligence_api INTERFACE "include")
endif()

target_compile_features(muhle_intelligence PRIVATE cxx_std_17)
set_target_properties(muhle_intelligence PROPERTIES CXX_EXTENSIONS OFF)

if(UNIX)
    target_compile_options(muhle_intelligence PRIVATE "-Wno-parentheses")
endif()

enable_warnings(muhle_intelligence)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(muhle_intelligence PRIVATE "NDEBUG")
endif()
