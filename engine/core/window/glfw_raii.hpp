#pragma once

#include <GLFW/glfw3.h>
#include <memory>

namespace r2d2 {

class GlfwInit {
public:
    GlfwInit();
    ~GlfwInit();
    GlfwInit(const GlfwInit&) = delete;
    GlfwInit& operator=(const GlfwInit&) = delete;

    static GlfwInit& instance();

private:
    bool initialized_ = false;
};

class GlfwWindow {
public:
    explicit GlfwWindow(GLFWwindow* w) noexcept;
    ~GlfwWindow();

    GlfwWindow(const GlfwWindow&) = delete;
    GlfwWindow& operator=(const GlfwWindow&) = delete;
    GlfwWindow(GlfwWindow&&) = delete;

    void* native_handle() const noexcept;
    void swap_buffers() noexcept;
    GLFWwindow* get() const noexcept;

    // Create a window or return nullptr on failure
    static std::unique_ptr<GlfwWindow> create(int width, int height, const char* title, bool fullscreen, bool resizable, int monitor_index = 0);

private:
    GLFWwindow* window_ = nullptr;
};

} // namespace r2d2
