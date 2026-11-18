#include "MeshGL3.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "GLContext.hpp"

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

void MeshGL3::draw() {
    // No vertex-data upload API exists yet. Do not issue a draw call using
    // uninitialized vertex attributes; this remains an allocation prototype.
    if (!owns_context() || !uploaded_)
        return;
}
