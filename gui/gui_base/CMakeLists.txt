cmake_minimum_required(VERSION 3.20)

add_library(gui_base STATIC
    "include/gui_base.hpp"
    "src/gui_base.cpp"
)

target_link_libraries(gui_base PRIVATE glfw)
target_link_libraries(gui_base PUBLIC imgui)
target_link_libraries(gui_base PUBLIC glad)

target_include_directories(gui_base PUBLIC "include")

target_compile_features(gui_base PRIVATE cxx_std_17)
set_target_properties(gui_base PROPERTIES CXX_EXTENSIONS OFF)

enable_warnings(gui_base)
