#pragma once

#include <thread>

struct ShaderGL3 {
    ShaderGL3();
    ~ShaderGL3();

    bool load(const char* vs_path, const char* fs_path);
    void use();

    void set_context_owner(std::thread::id owner) noexcept;
    bool owns_context() const noexcept;

private:
    // OpenGL objects are only accessed from the context-owning thread.
    std::thread::id owner_thread_{};
    unsigned int program_{0};
    bool compiled_{false};
};
