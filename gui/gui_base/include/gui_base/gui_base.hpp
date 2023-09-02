#pragma once

// Include this for the user
#include <imgui.h>

struct GLFWwindow;

namespace gui_base {
    class GuiApplication {
    public:
        GuiApplication() = default;
        GuiApplication(int width, int height, const char* title, bool resizable = true);
        virtual ~GuiApplication() = default;

        int run();
    protected:
        virtual void start() = 0;
        virtual void update() = 0;
        virtual void dispose() = 0;

        void quit();

        int exit_code = 0;

        struct {
            int width = 1280;
            int height = 720;
            const char* title = "GUI Base";
            bool resizable = true;
        } window_properties;
    private:
        void loop();
        void initialize();
        void uninitialize();

        GLFWwindow* window = nullptr;
    };
}
