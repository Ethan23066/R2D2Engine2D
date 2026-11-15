#include "window.hpp"
#include "glfw_raii.hpp"

#include <iostream>
#include <memory>
#include <mutex>

namespace r2d2 {

WindowState::WindowState() = default;
WindowState::~WindowState() = default;

void WindowState::set_owner(std::thread::id owner) noexcept {
    std::scoped_lock lock(mutex_);
    owner_thread_ = owner;
}

std::thread::id WindowState::owner() const noexcept {
    std::scoped_lock lock(mutex_);
    return owner_thread_;
}

void WindowState::set_native_handle(void* handle) noexcept {
    std::scoped_lock lock(mutex_);
    native_handle_ = handle;
}

void* WindowState::native_handle() const noexcept {
    std::scoped_lock lock(mutex_);
    return native_handle_;
}

bool WindowState::is_owner_thread() const noexcept {
    const auto current = std::this_thread::get_id();
    const auto owner_id = owner();
    return owner_id == std::thread::id{} || owner_id == current;
}

} // namespace r2d2

// GLFW window operations are confined to the thread that created the window.
// Avoid taking a global mutex for every poll/close/swap operation.
static std::unique_ptr<r2d2::GlfwWindow> g_window_ptr = nullptr;

int engine_window_select_monitor(const EngineMonitorMode* modes, int count)
{
    if (!modes || count <= 0)
        return 0;

    if (count == 2)
        return 1;

    int best_index = 0;
    int best_area = modes[0].width * modes[0].height;

    for (int i = 1; i < count; ++i) {
        const int area = modes[i].width * modes[i].height;
        if (area > best_area) {
            best_area = area;
            best_index = i;
        }
    }

    return best_index;
}

int engine_window_init(const EngineWindowConfig* cfg, int monitor_index)
{
    if (!cfg || g_window_ptr)
        return 0;

    g_window_ptr = r2d2::GlfwWindow::create(cfg->width, cfg->height, cfg->title,
                                             cfg->fullscreen, cfg->resizable, monitor_index);
    if (!g_window_ptr) {
        std::cerr << "Failed to create GLFW window\n";
        return 0;
    }

    return 1;
}

void engine_window_run()
{
    // This helper has no per-frame rendering; block instead of busy-spinning.
    while (!engine_window_should_close())
        glfwWaitEvents();
}

bool engine_window_should_close()
{
    if (!g_window_ptr)
        return true;

    GLFWwindow* w = g_window_ptr->get();
    if (!w)
        return true;

    return glfwWindowShouldClose(w) == GLFW_TRUE;
}

void engine_window_swap_buffers()
{
    if (g_window_ptr)
        g_window_ptr->swap_buffers();
}

void engine_window_poll_events()
{
    glfwPollEvents();
}

void engine_window_shutdown()
{
    g_window_ptr.reset();
}

void* engine_window_get_native_handle()
{
    return g_window_ptr ? g_window_ptr->native_handle() : nullptr;
}
