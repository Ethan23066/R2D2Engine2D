#pragma once

#include <thread>

class RendererGL3 {
public:
    RendererGL3();

    int init(void* win, int w, int h);
    void begin_frame(float r, float g, float b, float a);
    void end_frame();
    void shutdown();

    void set_context_owner(std::thread::id owner) noexcept;
    bool owns_context() const noexcept;
    bool is_current_thread_owner() const noexcept;

private:
    // OpenGL calls and context transitions are confined to this thread.
    void* window = nullptr;
    int width = 0;
    int height = 0;
    std::thread::id owner_thread_{};
};
