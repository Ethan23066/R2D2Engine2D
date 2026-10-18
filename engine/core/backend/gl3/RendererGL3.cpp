#include "RendererGL3.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

RendererGL3::RendererGL3() {}

int RendererGL3::init(void* win, int w, int h)
{
    this->window = win;
    this->width = w;
    this->height = h;

    if (!win) {
        std::cerr << "Renderer init: null window handle\n";
        return 0;
    }

    // Ensure the OpenGL context is current on this thread before loading GL function pointers
    GLFWwindow* glfw_window = static_cast<GLFWwindow*>(win);
    glfwMakeContextCurrent(glfw_window);

    if (!gladLoadGLLoader(
            reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to load OpenGL via GLAD\n";
        return 0;
    }

    glViewport(0, 0, w, h);

    return 1;
}

void RendererGL3::begin_frame(float r, float g, float b, float a)
{
    if (!window)
        return;

    GLFWwindow* glfw_window =
        static_cast<GLFWwindow*>(window);

    int framebuffer_width = 0;
    int framebuffer_height = 0;

    glfwGetFramebufferSize(
        glfw_window,
        &framebuffer_width,
        &framebuffer_height
    );

    if (framebuffer_width != width ||
        framebuffer_height != height) {
        width = framebuffer_width;
        height = framebuffer_height;
        glViewport(0, 0, width, height);
    }

    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void RendererGL3::end_frame()
{
    if (window)
        glfwSwapBuffers(
            static_cast<GLFWwindow*>(window)
        );
}

void RendererGL3::shutdown()
{
    window = nullptr;
}
