#pragma once

#include <thread>

struct Texture {
    Texture();
    ~Texture();

    bool load(const char* path);
    void bind(int unit);

    void set_context_owner(std::thread::id owner) noexcept;
    bool owns_context() const noexcept;

private:
    // OpenGL objects are only accessed from the context-owning thread.
    std::thread::id owner_thread_{};
    unsigned int texture_id_{0};
    bool loaded_{false};
};
