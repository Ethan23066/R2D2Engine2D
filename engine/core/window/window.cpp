#include "window.hpp"
#include "glfw_raii.hpp"
#include <iostream>
#include <memory>
#include <thread>
namespace r2d2 {
WindowState::WindowState() = default;
WindowState::~WindowState() = default;
void WindowState::set_owner(std::thread::id owner) noexcept { std::scoped_lock lock(mutex_); owner_thread_ = owner; }
std::thread::id WindowState::owner() const noexcept { std::scoped_lock lock(mutex_); return owner_thread_; }
void WindowState::set_native_handle(void* handle) noexcept { std::scoped_lock lock(mutex_); native_handle_ = handle; }
void* WindowState::native_handle() const noexcept { std::scoped_lock lock(mutex_); return native_handle_; }
bool WindowState::is_owner_thread() const noexcept {
    const auto id = owner(); return id == std::thread::id{} || id == std::this_thread::get_id();
}
} // namespace r2d2

// GLFW event/window operations stay on the thread that created the window.
static std::unique_ptr<r2d2::GlfwWindow> g_window_ptr;
static std::thread::id g_window_owner{};
static bool on_window_thread() noexcept {
    return g_window_owner == std::thread::id{} || g_window_owner == std::this_thread::get_id();
}
int engine_window_select_monitor(const EngineMonitorMode* modes, int count) {
    if (!modes || count <= 0) return 0;
    int best_index = 0;
    long long best_area = static_cast<long long>(modes[0].width) * modes[0].height;
    for (int i = 1; i < count; ++i) {
        const long long area = static_cast<long long>(modes[i].width) * modes[i].height;
        if (area > best_area) { best_area = area; best_index = i; }
    }
    return best_index;
}
int engine_window_init(const EngineWindowConfig* cfg, int monitor_index) {
    if (!cfg || g_window_ptr) return 0;
    auto window = r2d2::GlfwWindow::create(cfg->width, cfg->height, cfg->title,
                                           cfg->fullscreen, cfg->resizable, monitor_index);
    if (!window) { std::cerr << "Failed to create GLFW window\n"; return 0; }
    g_window_owner = std::this_thread::get_id();
    g_window_ptr = std::move(window);
    return 1;
}
void engine_window_run() {
    if (!on_window_thread()) return;
    while (!engine_window_should_close()) glfwWaitEvents();
}
bool engine_window_should_close() {
    if (!on_window_thread() || !g_window_ptr) return true;
    GLFWwindow* w = g_window_ptr->get();
    return !w || glfwWindowShouldClose(w) == GLFW_TRUE;
}
void engine_window_swap_buffers() { if (on_window_thread() && g_window_ptr) g_window_ptr->swap_buffers(); }
void engine_window_poll_events() { if (on_window_thread()) glfwPollEvents(); }
void engine_window_shutdown() {
    if (!on_window_thread()) return;
    g_window_ptr.reset(); g_window_owner = std::thread::id{};
}
void* engine_window_get_native_handle() {
    return on_window_thread() && g_window_ptr ? g_window_ptr->native_handle() : nullptr;
}
