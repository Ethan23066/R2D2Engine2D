#include "RendererGL3.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "engine/core/window/window.hpp"
#include "GLContext.hpp"

#include <iostream>
#include <thread>

RendererGL3::RendererGL3() = default;

RendererGL3::~RendererGL3() {
    shutdown();
}

bool RendererGL3::is_window_still_valid() const noexcept {
    return window != nullptr && engine_window_get_native_handle() == window;
}

void RendererGL3::set_context_owner(std::thread::id owner) noexcept {
    owner_thread_ = owner;
}

bool RendererGL3::owns_context() const noexcept {
    return window
        && owner_thread_ == std::this_thread::get_id()
        && r2d2::gl3_detail::is_current_context_owner(glfwGetCurrentContext());
}

bool RendererGL3::is_current_thread_owner() const noexcept {
    return window && owner_thread_ == std::this_thread::get_id()
        && r2d2::gl3_detail::is_context_owner_thread();
}

int RendererGL3::init(void* win, int w, int h) {
    if (!win || w <= 0 || h <= 0 || window || r2d2::gl3_detail::context_active()) {
        std::cerr << "Renderer init: invalid window/dimensions or an OpenGL context is already registered\n";
        return 0;
    }

    GLFWwindow* glfw_window = static_cast<GLFWwindow*>(win);
    glfwMakeContextCurrent(glfw_window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to load OpenGL via GLAD\n";
        glfwMakeContextCurrent(nullptr);
        return 0;
    }

    const auto current_thread = std::this_thread::get_id();
    if (!r2d2::gl3_detail::activate_context(current_thread, win)) {
        glfwMakeContextCurrent(nullptr);
        std::cerr << "Unable to register the OpenGL context owner\n";
        return 0;
    }

    window = win;
    width = w;
    height = h;
    owner_thread_ = current_thread;
    glViewport(0, 0, w, h);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return 1;
}

void RendererGL3::begin_frame(float r, float g, float b, float a) {
    if (!is_window_still_valid() || owner_thread_ != std::this_thread::get_id()
        || !r2d2::gl3_detail::is_context_owner_thread())
        return;

    auto* glfw_window = static_cast<GLFWwindow*>(window);
    if (glfwGetCurrentContext() != glfw_window)
        glfwMakeContextCurrent(glfw_window);

    if (!owns_context())
        return;

    int framebuffer_width = 0;
    int framebuffer_height = 0;
    glfwGetFramebufferSize(glfw_window, &framebuffer_width, &framebuffer_height);
    if (framebuffer_width <= 0 || framebuffer_height <= 0)
        return;

    if (framebuffer_width != width || framebuffer_height != height) {
        width = framebuffer_width;
        height = framebuffer_height;
        glViewport(0, 0, width, height);
    }

    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void RendererGL3::end_frame() {
    if (!is_window_still_valid() || owner_thread_ != std::this_thread::get_id()
        || !r2d2::gl3_detail::is_context_owner_thread())
        return;

    auto* glfw_window = static_cast<GLFWwindow*>(window);
    if (glfwGetCurrentContext() != glfw_window)
        glfwMakeContextCurrent(glfw_window);

    if (owns_context())
        glfwSwapBuffers(glfw_window);
}

void RendererGL3::shutdown() {
    if (!window || owner_thread_ != std::this_thread::get_id())
        return;

    if (engine_window_get_native_handle() == window) {
        auto* glfw_window = static_cast<GLFWwindow*>(window);
        glfwMakeContextCurrent(glfw_window);
        r2d2::gl3_detail::deactivate_context(owner_thread_, window);
        glfwMakeContextCurrent(nullptr);
    } else {
        r2d2::gl3_detail::deactivate_context(owner_thread_, window);
    }

    window = nullptr;
    width = 0;
    height = 0;
    owner_thread_ = {};
}
