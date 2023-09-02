#include <cstdlib>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include "gui_base/gui_base.hpp"

namespace gui_base {
    GuiApplication::GuiApplication(int width, int height, const char* title, bool resizable) {
        window_properties.width = width;
        window_properties.height = height;
        window_properties.title = title;
        window_properties.resizable = resizable;
    }

    int GuiApplication::run() {
        initialize();
        start();

        loop();

        dispose();
        uninitialize();

        return exit_code;
    }

    void GuiApplication::quit() {
        glfwSetWindowShouldClose(window, 1);
    }

    void GuiApplication::loop() {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            update();

            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }
    }

    void GuiApplication::initialize() {
        if (!glfwInit()) {
            std::exit(1);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, window_properties.resizable ? GLFW_TRUE : GLFW_FALSE);

        window = glfwCreateWindow(
            window_properties.width,
            window_properties.height,
            window_properties.title,
            nullptr,
            nullptr
        );

        if (window == nullptr) {
            glfwTerminate();
            std::exit(1);
        }

        glfwMakeContextCurrent(window);

        if (!gladLoadGL()) {
            glfwTerminate();
            std::exit(1);
        }

        glfwSwapInterval(1);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 430 core");
    }

    void GuiApplication::uninitialize() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
    }
}
