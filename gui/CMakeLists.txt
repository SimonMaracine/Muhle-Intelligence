cmake_minimum_required(VERSION 3.20)

add_subdirectory(extern/glm)
add_subdirectory(extern/just-dl)
add_subdirectory(extern/tiny-gui-base)
add_subdirectory(extern/ImGuiFileDialog)

target_include_directories(ImGuiFileDialog PUBLIC "${tiny-gui-base_SOURCE_DIR}/extern/imgui")  # TODO move to tiny-gui-base

add_subdirectory(muhle_tester)
