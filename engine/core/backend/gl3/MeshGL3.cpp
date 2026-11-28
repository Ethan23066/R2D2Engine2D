#include "MeshGL3.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "GLContext.hpp"

#include <array>
#include <limits>
#include <thread>

bool MeshGL3::owns_context() const noexcept {
    return owner_thread_ == std::this_thread::get_id()
        && r2d2::gl3_detail::is_current_context_owner(glfwGetCurrentContext());
}

MeshGL3::~MeshGL3() {
    if (!owns_context())
        return;

    if (vbo_)
        glDeleteBuffers(1, &vbo_);
    if (vao_)
        glDeleteVertexArrays(1, &vao_);
}

void MeshGL3::set_context_owner(std::thread::id owner) noexcept {
    owner_thread_ = owner;
}

void MeshGL3::upload() {
    if (!r2d2::gl3_detail::is_current_context_owner(glfwGetCurrentContext()) || uploaded_)
        return;

    const auto current_thread = std::this_thread::get_id();
    if (owner_thread_ == std::thread::id{})
        owner_thread_ = current_thread;
    if (owner_thread_ != current_thread)
        return;

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    if (!vao_ || !vbo_) {
        if (vbo_) glDeleteBuffers(1, &vbo_);
        if (vao_) glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
        vbo_ = 0;
        return;
    }

    uploaded_ = true;
}

bool MeshGL3::upload_vertices(std::span<const float> vertices) {
    // Five floats per vertex: position (x, y, z) and texture coordinates (u, v).
    if (vertices.empty() || vertices.size() % 5 != 0)
        return false;

    const std::size_t vertex_count = vertices.size() / 5;
    if (vertex_count > static_cast<std::size_t>(std::numeric_limits<int>::max())
        || vertices.size_bytes() > static_cast<std::size_t>(std::numeric_limits<GLsizeiptr>::max()))
        return false;

    if (!uploaded_)
        upload();
    if (!owns_context() || !uploaded_)
        return false;

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices.size_bytes()),
        vertices.data(),
        GL_STATIC_DRAW
    );

    constexpr GLsizei stride = 5 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    vertex_count_ = static_cast<int>(vertex_count);
    return true;
}

bool MeshGL3::upload_quad(float width, float height) {
    if (!std::isfinite(width) || !std::isfinite(height) || width <= 0.0f || height <= 0.0f)
        return false;

    const float hw = width * 0.5f;
    const float hh = height * 0.5f;

    const std::array<float, 30> quad = {
        -hw, -hh, 0.0f,  0.0f, 0.0f,
         hw, -hh, 0.0f,  1.0f, 0.0f,
         hw,  hh, 0.0f,  1.0f, 1.0f,

        -hw, -hh, 0.0f,  0.0f, 0.0f,
         hw,  hh, 0.0f,  1.0f, 1.0f,
        -hw,  hh, 0.0f,  0.0f, 1.0f
    };

    return upload_vertices(quad);
}

void MeshGL3::draw() {
    if (!owns_context() || !uploaded_ || vertex_count_ <= 0)
        return;

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, vertex_count_);
    glBindVertexArray(0);
}
