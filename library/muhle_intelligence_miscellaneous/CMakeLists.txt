cmake_minimum_required(VERSION 3.20)

add_library(muhle_intelligence_miscellaneous STATIC
    "include/muhle_intelligence/miscellaneous.hpp"
    "src/miscellaneous.cpp"
)

target_link_libraries(muhle_intelligence_miscellaneous PUBLIC muhle_intelligence_definitions)

target_include_directories(muhle_intelligence_miscellaneous PUBLIC "include")

target_compile_features(muhle_intelligence_miscellaneous PRIVATE cxx_std_17)
set_target_properties(muhle_intelligence_miscellaneous PROPERTIES CXX_EXTENSIONS OFF)

enable_warnings(muhle_intelligence_miscellaneous)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(muhle_intelligence_miscellaneous PRIVATE "NDEBUG")
endif()
