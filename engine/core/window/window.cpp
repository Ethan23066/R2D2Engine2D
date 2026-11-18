#include "window.hpp"
#include "glfw_raii.hpp"

#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

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
    const auto id = owner();
    return id == std::thread::id{} || id == std::this_thread::get_id();
}

} // namespace r2d2

namespace {
std::mutex g_window_mutex;
std::unique_ptr<r2d2::GlfwWindow> g_window_ptr;
std::thread::id g_window_owner{};
bool g_shutdown_requested = false;

void process_pending_shutdown() {
    std::lock_guard lock(g_window_mutex);
    if (g_window_ptr
        && g_window_owner == std::this_thread::get_id()
        && g_shutdown_requested) {
        g_window_ptr.reset();
        g_window_owner = {};
        g_shutdown_requested = false;
    }
}
} // namespace

int engine_window_select_monitor(const EngineMonitorMode* modes, int count) {
    if (!modes || count <= 0)
        return 0;

    int best_index = 0;
    long long best_area = static_cast<long long>(modes[0].width) * modes[0].height;
    for (int i = 1; i < count; ++i) {
        const long long area = static_cast<long long>(modes[i].width) * modes[i].height;
        if (area > best_area) {
            best_area = area;
            best_index = i;
        }
    }
    return best_index;
}

int engine_window_init(const EngineWindowConfig* cfg, int monitor_index) {
    if (!cfg || cfg->width <= 0 || cfg->height <= 0)
        return 0;

    std::lock_guard lock(g_window_mutex);
    if (g_window_ptr) {
        std::cerr << "A GLFW window is already active\n";
        return 0;
    }

    auto window = r2d2::GlfwWindow::create(
        cfg->width, cfg->height, cfg->title,
        cfg->fullscreen, cfg->resizable, monitor_index);

    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        return 0;
    }

    g_window_owner = std::this_thread::get_id();
    g_shutdown_requested = false;
    g_window_ptr = std::move(window);
    return 1;
}

void engine_window_run() {
    for (;;) {
        if (engine_window_should_close())
            break;
        glfwWaitEvents();
        process_pending_shutdown();
    }
}

bool engine_window_should_close() {
    process_pending_shutdown();

    std::lock_guard lock(g_window_mutex);
    if (!g_window_ptr
        || g_window_owner != std::this_thread::get_id()
        || g_shutdown_requested)
        return true;

    GLFWwindow* window = g_window_ptr->get();
    return !window || glfwWindowShouldClose(window) == GLFW_TRUE;
}

void engine_window_swap_buffers() {
    std::lock_guard lock(g_window_mutex);
    if (!g_window_ptr || g_window_owner != std::this_thread::get_id()
        || g_shutdown_requested)
        return;

    g_window_ptr->swap_buffers();
}

void engine_window_poll_events() {
    {
        std::lock_guard lock(g_window_mutex);
        if (!g_window_ptr || g_window_owner != std::this_thread::get_id())
            return;
    }

    glfwPollEvents();
    process_pending_shutdown();
}

void engine_window_shutdown() {
    bool wake_owner = false;
    {
        std::lock_guard lock(g_window_mutex);
        if (!g_window_ptr)
            return;

        if (g_window_owner == std::this_thread::get_id()) {
            g_window_ptr.reset();
            g_window_owner = {};
            g_shutdown_requested = false;
            return;
        }

        // GLFW window destruction must happen on the owner thread. Ask that
        // thread to finish destruction on its next event-loop iteration.
        g_shutdown_requested = true;
        wake_owner = true;
    }

    if (wake_owner)
        glfwPostEmptyEvent();
}

void* engine_window_get_native_handle() {
    std::lock_guard lock(g_window_mutex);
    if (!g_window_ptr || g_window_owner != std::this_thread::get_id()
        || g_shutdown_requested)
        return nullptr;

    return g_window_ptr->native_handle();
}
