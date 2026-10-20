#include "glfw_raii.hpp"

#include <GLFW/glfw3.h>
#include <stdexcept>
#include <vector>

namespace r2d2 {

GlfwInit::GlfwInit() {
    std::scoped_lock lock(init_mutex_);
    initialized_ = (glfwInit() == GLFW_TRUE);
    if (!initialized_)
        throw std::runtime_error("glfwInit failed");
}

GlfwInit::~GlfwInit() {
    std::scoped_lock lock(init_mutex_);
    if (initialized_)
        glfwTerminate();
}

GlfwInit& GlfwInit::instance() {
    static GlfwInit inst;
    return inst;
}

GlfwWindow::GlfwWindow(GLFWwindow* w) noexcept : window_(w) {
    owner_thread_ = std::this_thread::get_id();
}

GlfwWindow::~GlfwWindow() {
    std::scoped_lock lock(mutex_);
    if (window_)
        glfwDestroyWindow(window_);
}

void* GlfwWindow::native_handle() const noexcept {
    std::scoped_lock lock(mutex_);
    return static_cast<void*>(window_);
}

void GlfwWindow::swap_buffers() noexcept {
    std::scoped_lock lock(mutex_);
    if (window_)
        glfwSwapBuffers(window_);
}

GLFWwindow* GlfwWindow::get() const noexcept {
    std::scoped_lock lock(mutex_);
    return window_;
}

void GlfwWindow::set_owner(std::thread::id owner) noexcept {
    std::scoped_lock lock(mutex_);
    owner_thread_ = owner;
}

bool GlfwWindow::owns_current_thread() const noexcept {
    const auto current = std::this_thread::get_id();
    std::scoped_lock lock(mutex_);
    return owner_thread_ == std::thread::id{} || owner_thread_ == current;
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

    auto window = std::unique_ptr<GlfwWindow>(new GlfwWindow(w));
    window->set_owner(std::this_thread::get_id());
    return window;
}

} // namespace r2d2
