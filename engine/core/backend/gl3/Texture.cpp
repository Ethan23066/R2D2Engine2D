#include "Texture.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#if defined(R2D2_USE_GLES)
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include "GLContext.hpp"

#include <thread>

bool Texture::owns_context() const noexcept {
    return owner_thread_ == std::this_thread::get_id()
        && r2d2::gl3_detail::is_current_context_owner(glfwGetCurrentContext());
}

Texture::~Texture() {
    if (texture_id_ && owns_context())
        glDeleteTextures(1, &texture_id_);
}

void Texture::set_context_owner(std::thread::id owner) noexcept {
    owner_thread_ = owner;
}

bool Texture::load(const char* path) {
    if (!path || !*path
        || !r2d2::gl3_detail::is_current_context_owner(glfwGetCurrentContext()))
        return false;

    const auto current_thread = std::this_thread::get_id();
    if (owner_thread_ == std::thread::id{})
        owner_thread_ = current_thread;
    if (owner_thread_ != current_thread)
        return false;

    // No image decoder is part of this backend yet.
    return false;
}

void Texture::bind(int unit) {
    if (unit < 0 || !owns_context() || !loaded_ || !texture_id_)
        return;

    GLint max_units = 0;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_units);
    if (unit >= max_units)
        return;

    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
}
