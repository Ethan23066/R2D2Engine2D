#include "MeshGL3.hpp"

#include <glad/glad.h>
#include <thread>

MeshGL3::MeshGL3() = default;

MeshGL3::~MeshGL3() = default;

void MeshGL3::set_context_owner(std::thread::id owner) noexcept {
    owner_thread_ = owner;
}

bool MeshGL3::owns_context() const noexcept {
    return owner_thread_ == std::thread::id{} ||
           owner_thread_ == std::this_thread::get_id();
}

void MeshGL3::upload() {
    if (!owns_context() || uploaded_)
        return;

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    uploaded_ = true;
}

void MeshGL3::draw() {
    if (!owns_context() || !uploaded_)
        return;

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
