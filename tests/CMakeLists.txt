cmake_minimum_required(VERSION 3.20)

set(INSTALL_GTEST OFF)
set(BUILD_GMOCK OFF)

add_subdirectory(extern/googletest)

add_executable(muhle_intelligence_test "main.cpp")

target_link_libraries(muhle_intelligence_test PRIVATE GTest::gtest_main)
target_link_libraries(muhle_intelligence_test PRIVATE muhle_intelligence_interface)

if(UNIX)
    target_compile_options(muhle_intelligence_test PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Wconversion")
elseif(MSVC)
    target_compile_options(muhle_intelligence_test PRIVATE "/W4")
else()
    message(WARNING "Warnings are not enabled")
endif()

target_compile_features(muhle_intelligence_test PRIVATE cxx_std_17)
set_target_properties(muhle_intelligence_test PROPERTIES CXX_EXTENSIONS OFF)
