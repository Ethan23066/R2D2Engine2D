#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <memory>
#include <mutex>
#include <thread>

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
    std::mutex init_mutex_;
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

    static std::unique_ptr<GlfwWindow> create(int width, int height, const char* title, bool fullscreen, bool resizable, int monitor_index = 0);

    void set_owner(std::thread::id owner) noexcept;
    bool owns_current_thread() const noexcept;

private:
    GLFWwindow* window_ = nullptr;
    std::thread::id owner_thread_{};
    mutable std::mutex mutex_;
};

} // namespace r2d2
