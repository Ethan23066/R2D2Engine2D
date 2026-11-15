#pragma once
#include <thread>
struct MeshGL3 {
    MeshGL3() = default;
    ~MeshGL3();
    MeshGL3(const MeshGL3&) = delete;
    MeshGL3& operator=(const MeshGL3&) = delete;
    void upload();
    void draw();
    void set_context_owner(std::thread::id owner) noexcept;
    bool owns_context() const noexcept;
private:
    std::thread::id owner_thread_{};
    bool uploaded_{false};
    unsigned int vao_{0};
    unsigned int vbo_{0};
};
