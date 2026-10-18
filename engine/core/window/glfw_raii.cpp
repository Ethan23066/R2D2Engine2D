#include "glfw_raii.hpp"

#include <GLFW/glfw3.h>
#include <stdexcept>
#include <vector>

namespace r2d2 {

GlfwInit::GlfwInit() {
    initialized_ = (glfwInit() == GLFW_TRUE);
    if (!initialized_) {
        // Do not throw during static init paths; caller can query via instance
        throw std::runtime_error("glfwInit failed");
    }
}

GlfwInit::~GlfwInit() {
    if (initialized_)
        glfwTerminate();
}

GlfwInit& GlfwInit::instance() {
    static GlfwInit inst;
    return inst;
}

GlfwWindow::GlfwWindow(GLFWwindow* w) noexcept : window_(w) {}

GlfwWindow::~GlfwWindow() {
    if (window_)
        glfwDestroyWindow(window_);
}

void* GlfwWindow::native_handle() const noexcept {
    return static_cast<void*>(window_);
}

void GlfwWindow::swap_buffers() noexcept {
    if (window_)
        glfwSwapBuffers(window_);
}

GLFWwindow* GlfwWindow::get() const noexcept {
    return window_;
}

std::unique_ptr<GlfwWindow> GlfwWindow::create(int width, int height, const char* title, bool fullscreen, bool resizable, int monitor_index) {
    try {
        (void)GlfwInit::instance();
    } catch (...) {
        return nullptr;
    }

    GLFWmonitor* monitor = nullptr;
    if (fullscreen) {
        int count = 0;
        GLFWmonitor** monitors = glfwGetMonitors(&count);
        if (count > 0) {
            if (monitor_index < 0) monitor_index = 0;
            if (monitor_index >= count) monitor_index = count - 1;
            monitor = monitors[monitor_index];
        }
    }

    glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);
    GLFWwindow* w = glfwCreateWindow(width, height, title ? title : "R2D2", monitor, nullptr);
    if (!w)
        return nullptr;

    return std::unique_ptr<GlfwWindow>(new GlfwWindow(w));
}

} // namespace r2d2
