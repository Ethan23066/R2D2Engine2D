#pragma once

#include <mutex>
#include <thread>

struct MeshGL3 {
    MeshGL3();
    ~MeshGL3();

    void upload();
    void draw();

    void set_context_owner(std::thread::id owner) noexcept;
    bool owns_context() const noexcept;

private:
    std::thread::id owner_thread_{};
    mutable std::mutex gl_mutex_;
    bool uploaded_{false};
    unsigned int vao_{0};
    unsigned int vbo_{0};
};
