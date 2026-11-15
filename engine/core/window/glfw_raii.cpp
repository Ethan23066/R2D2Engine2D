#include "glfw_raii.hpp"
#include <iostream>
#include <stdexcept>
namespace r2d2 {
GlfwInit::GlfwInit() {
    std::scoped_lock lock(init_mutex_);
    initialized_ = glfwInit() == GLFW_TRUE;
    if (!initialized_) throw std::runtime_error("glfwInit failed");
}
GlfwInit::~GlfwInit() {
    std::scoped_lock lock(init_mutex_);
    if (initialized_) glfwTerminate();
}
GlfwInit& GlfwInit::instance() {
    static GlfwInit inst;
    return inst;
}
GlfwWindow::GlfwWindow(GLFWwindow* w) noexcept
    : window_(w), owner_thread_(std::this_thread::get_id()) {}
GlfwWindow::~GlfwWindow() {
    if (!window_) return;
    if (!owns_current_thread()) {
        // GLFW destruction from another thread is undefined. Keep the handle
        // untouched rather than making an invalid GLFW call.
        std::cerr << "GlfwWindow destroyed from a non-owner thread; window was not destroyed\n";
        return;
    }
    glfwDestroyWindow(window_);
    window_ = nullptr;
}
void* GlfwWindow::native_handle() const noexcept {
    return owns_current_thread() ? static_cast<void*>(window_) : nullptr;
}
void GlfwWindow::swap_buffers() noexcept {
    if (window_ && owns_current_thread()) glfwSwapBuffers(window_);
}
GLFWwindow* GlfwWindow::get() const noexcept {
    return owns_current_thread() ? window_ : nullptr;
}
void GlfwWindow::set_owner(std::thread::id owner) noexcept { owner_thread_ = owner; }
bool GlfwWindow::owns_current_thread() const noexcept {
    return owner_thread_ == std::thread::id{} || owner_thread_ == std::this_thread::get_id();
}
std::unique_ptr<GlfwWindow> GlfwWindow::create(int width, int height, const char* title,
    bool fullscreen, bool resizable, int monitor_index) {
    try { (void)GlfwInit::instance(); }
    catch (...) { return nullptr; }
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
    if (!w) return nullptr;
    return std::unique_ptr<GlfwWindow>(new GlfwWindow(w));
}
} // namespace r2d2
