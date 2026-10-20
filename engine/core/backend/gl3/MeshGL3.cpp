#include "MeshGL3.hpp"

#include <glad/glad.h>
#include <thread>

MeshGL3::MeshGL3() = default;

MeshGL3::~MeshGL3() = default;

void MeshGL3::set_context_owner(std::thread::id owner) noexcept {
    std::scoped_lock lock(gl_mutex_);
    owner_thread_ = owner;
}

bool MeshGL3::owns_context() const noexcept {
    const auto current = std::this_thread::get_id();
    std::scoped_lock lock(gl_mutex_);
    return owner_thread_ == std::thread::id{} || owner_thread_ == current;
}

void MeshGL3::upload() {
    if (!owns_context())
        return;

    std::scoped_lock lock(gl_mutex_);
    if (uploaded_)
        return;

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    uploaded_ = true;
}

void MeshGL3::draw() {
    if (!owns_context())
        return;

    std::scoped_lock lock(gl_mutex_);
    if (!uploaded_)
        return;

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
