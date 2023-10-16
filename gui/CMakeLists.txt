cmake_minimum_required(VERSION 3.20)

set(GUI_BASE_INCLUDE_FILE_DIALOG ON)

add_subdirectory(extern/just-dl)
add_subdirectory(extern/tiny-gui-base)

add_subdirectory(common)
add_subdirectory(muhle_player)
# add_subdirectory(muhle_tester)
