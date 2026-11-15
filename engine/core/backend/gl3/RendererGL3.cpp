#include "RendererGL3.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <thread>
RendererGL3::RendererGL3() = default;
void RendererGL3::set_context_owner(std::thread::id owner) noexcept { owner_thread_ = owner; }
bool RendererGL3::owns_context() const noexcept {
    return owner_thread_ == std::thread::id{} || owner_thread_ == std::this_thread::get_id();
}
bool RendererGL3::is_current_thread_owner() const noexcept { return owns_context(); }
int RendererGL3::init(void* win, int w, int h) {
    if (!win || w <= 0 || h <= 0 || window) {
        std::cerr << "Renderer init: invalid window, dimensions, or renderer already initialized\n";
        return 0;
    }
    GLFWwindow* glfw_window = static_cast<GLFWwindow*>(win);
    glfwMakeContextCurrent(glfw_window);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to load OpenGL via GLAD\n";
        glfwMakeContextCurrent(nullptr);
        return 0;
    }
    window = win;
    width = w;
    height = h;
    owner_thread_ = std::this_thread::get_id();
    glViewport(0, 0, w, h);
    return 1;
}
void RendererGL3::begin_frame(float r, float g, float b, float a) {
    if (!window || !owns_context()) return;
    auto* glfw_window = static_cast<GLFWwindow*>(window);
    int framebuffer_width = 0, framebuffer_height = 0;
    glfwGetFramebufferSize(glfw_window, &framebuffer_width, &framebuffer_height);
    if (framebuffer_width <= 0 || framebuffer_height <= 0) return;
    if (framebuffer_width != width || framebuffer_height != height) {
        width = framebuffer_width;
        height = framebuffer_height;
        glViewport(0, 0, width, height);
    }
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}
void RendererGL3::end_frame() {
    if (!window || !owns_context() || width <= 0 || height <= 0) return;
    glfwSwapBuffers(static_cast<GLFWwindow*>(window));
}
void RendererGL3::shutdown() {
    if (!window || !owns_context()) return;
    glfwMakeContextCurrent(nullptr);
    window = nullptr;
    width = 0;
    height = 0;
    owner_thread_ = std::thread::id{};
}
