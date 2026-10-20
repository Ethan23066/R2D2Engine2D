#pragma once

#include <mutex>
#include <thread>

struct Texture {
    Texture();
    ~Texture();

    bool load(const char* path);
    void bind(int unit);

    void set_context_owner(std::thread::id owner) noexcept;
    bool owns_context() const noexcept;

private:
    std::thread::id owner_thread_{};
    mutable std::mutex gl_mutex_;
    unsigned int texture_id_{0};
    bool loaded_{false};
};
