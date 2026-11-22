#pragma once

#include <cstddef>
#include <span>
#include <thread>

struct MeshGL3 {
    MeshGL3() = default;
    ~MeshGL3();
    MeshGL3(const MeshGL3&) = delete;
    MeshGL3& operator=(const MeshGL3&) = delete;

    void upload();
    bool upload_vertices(std::span<const float> vertices);
    bool upload_quad(float width = 1.0f, float height = 1.0f);
    void draw();

    void set_context_owner(std::thread::id owner) noexcept;
    bool owns_context() const noexcept;
    int vertex_count() const noexcept { return vertex_count_; }

private:
    std::thread::id owner_thread_{};
    bool uploaded_{false};
    unsigned int vao_{0};
    unsigned int vbo_{0};
    int vertex_count_{0};
};
