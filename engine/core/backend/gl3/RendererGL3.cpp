#include "RendererGL3.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <thread>

RendererGL3::RendererGL3() = default;

void RendererGL3::set_context_owner(std::thread::id owner) noexcept {
    std::scoped_lock lock(gl_mutex_);
    owner_thread_ = owner;
}

bool RendererGL3::owns_context() const noexcept {
    const auto current = std::this_thread::get_id();
    std::scoped_lock lock(gl_mutex_);
    return owner_thread_ == std::thread::id{} || owner_thread_ == current;
}

int RendererGL3::init(void* win, int w, int h)
{
    if (!win) {
        std::cerr << "Renderer init: null window handle\n";
        return 0;
    }

    std::scoped_lock lock(gl_mutex_);
    window = win;
    width = w;
    height = h;
    owner_thread_ = std::this_thread::get_id();

    GLFWwindow* glfw_window = static_cast<GLFWwindow*>(win);
    glfwMakeContextCurrent(glfw_window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to load OpenGL via GLAD\n";
        return 0;
    }

    glViewport(0, 0, w, h);
    return 1;
}

void RendererGL3::begin_frame(float r, float g, float b, float a)
{
    if (!window || !owns_context())
        return;

    std::scoped_lock lock(gl_mutex_);
    GLFWwindow* glfw_window = static_cast<GLFWwindow*>(window);

    int framebuffer_width = 0;
    int framebuffer_height = 0;
    glfwGetFramebufferSize(glfw_window, &framebuffer_width, &framebuffer_height);

    if (framebuffer_width != width || framebuffer_height != height) {
        width = framebuffer_width;
        height = framebuffer_height;
        glViewport(0, 0, width, height);
    }

    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void RendererGL3::end_frame()
{
    if (!window || !owns_context())
        return;

    std::scoped_lock lock(gl_mutex_);
    glfwSwapBuffers(static_cast<GLFWwindow*>(window));
}

void RendererGL3::shutdown()
{
    std::scoped_lock lock(gl_mutex_);
    window = nullptr;
    owner_thread_ = std::thread::id{};
}
