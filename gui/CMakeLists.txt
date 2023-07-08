cmake_minimum_required(VERSION 3.20)

add_subdirectory(extern/glfw)

add_library(imgui STATIC
    "extern/imgui/imgui.h"
    "extern/imgui/imgui.cpp"
    "extern/imgui/imgui_widgets.cpp"
    "extern/imgui/imgui_tables.cpp"
    "extern/imgui/imgui_draw.cpp"
    "extern/imgui/imgui_demo.cpp"
    "extern/imgui/imgui_internal.h"

    "extern/imgui/backends/imgui_impl_glfw.h"
    "extern/imgui/backends/imgui_impl_glfw.cpp"
    "extern/imgui/backends/imgui_impl_opengl3.h"
    "extern/imgui/backends/imgui_impl_opengl3.cpp"
)

target_include_directories(imgui PUBLIC "extern/imgui")
target_include_directories(imgui PRIVATE "extern/glfw/include")

add_library(glad STATIC "extern/glad/src/glad.c")
target_include_directories(glad PUBLIC "extern/glad/include")

add_subdirectory(gui_base)
add_subdirectory(muhle_tester)
