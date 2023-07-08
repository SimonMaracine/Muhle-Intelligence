#pragma once

// Include these for the user
#include <imgui.h>
#include <glad/glad.h>

struct GLFWwindow;

namespace gui_base {
    class GuiApplication {
    public:
        GuiApplication() = default;
        virtual ~GuiApplication() = default;

        int run();
    protected:
        virtual void start() = 0;
        virtual void update() = 0;

        bool running = true;
        int exit_code = 0;
    private:
        void loop();
        void initialize();
        void uninitialize();

        GLFWwindow* window = nullptr;
    };
}
