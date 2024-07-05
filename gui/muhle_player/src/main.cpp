#include <iostream>

#include <gui_base/gui_base.hpp>

#include "muhle_player.hpp"

int main() {
    gui_base::WindowProperties properties;
    properties.width = 1024;
    properties.height = 576;
    properties.title = u8"Mühle Player";

    try {
        MuhlePlayer program {properties};
        return program.run();
    } catch (const gui_base::InitializationError& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
