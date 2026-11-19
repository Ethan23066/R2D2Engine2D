#include "RendererGL3.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#if defined(R2D2_USE_GLES)
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include "GLContext.hpp"

#include <iostream>
#include <thread>

RendererGL3::RendererGL3() = default;

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

#if !defined(R2D2_USE_GLES)
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to load OpenGL via GLAD\n";
        glfwMakeContextCurrent(nullptr);
        return 0;
    }
#endif

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
    return 1;
}

void RendererGL3::begin_frame(float r, float g, float b, float a) {
    if (!window || owner_thread_ != std::this_thread::get_id()
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
    if (!window || owner_thread_ != std::this_thread::get_id()
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

    auto* glfw_window = static_cast<GLFWwindow*>(window);
    glfwMakeContextCurrent(glfw_window);
    r2d2::gl3_detail::deactivate_context(owner_thread_, window);
    glfwMakeContextCurrent(nullptr);

    window = nullptr;
    width = 0;
    height = 0;
    owner_thread_ = {};
}
