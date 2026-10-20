#pragma once

#include <cstdint>
#include <mutex>
#include <thread>

struct EngineMonitorMode {
    int width;
    int height;
    int refresh_rate;
};

struct EngineWindowConfig {
    int width;
    int height;
    const char* title;
    bool fullscreen;
    bool resizable;
};

int engine_window_select_monitor(const EngineMonitorMode* modes, int count);

int engine_window_init(const EngineWindowConfig* cfg, int monitor_index = 0);
void engine_window_run();
bool engine_window_should_close();
void engine_window_swap_buffers();
void engine_window_poll_events();
void engine_window_shutdown();

void* engine_window_get_native_handle();

namespace r2d2 {
class WindowState {
public:
    WindowState();
    ~WindowState();

    WindowState(const WindowState&) = delete;
    WindowState& operator=(const WindowState&) = delete;

    void set_owner(std::thread::id owner) noexcept;
    std::thread::id owner() const noexcept;

    void set_native_handle(void* handle) noexcept;
    void* native_handle() const noexcept;

    bool is_owner_thread() const noexcept;

private:
    std::thread::id owner_thread_{};
    void* native_handle_ = nullptr;
    mutable std::mutex mutex_;
};
} // namespace r2d2
