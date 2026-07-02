#include "engine_window.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

static GLFWwindow* window = nullptr;

bool engine_window_init(EngineWindowConfig cfg)
{
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW\n";
        return false;
    }

    window = glfwCreateWindow(cfg.width, cfg.height, cfg.title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    return true;
}

void engine_window_run()
{
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void engine_window_shutdown()
{
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}
