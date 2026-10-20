#pragma once

#include <mutex>
#include <thread>

struct ShaderGL3 {
    ShaderGL3();
    ~ShaderGL3();

    bool load(const char* vs_path, const char* fs_path);
    void use();

    void set_context_owner(std::thread::id owner) noexcept;
    bool owns_context() const noexcept;

private:
    std::thread::id owner_thread_{};
    mutable std::mutex gl_mutex_;
    unsigned int program_{0};
    bool compiled_{false};
};
