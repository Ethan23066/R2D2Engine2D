#include "Texture.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "GLContext.hpp"

#include <iostream>
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

bool Texture::upload_rgba(const std::uint8_t* rgba_data, int w, int h, bool pixelated) {
    if (!rgba_data || w <= 0 || h <= 0
        || !r2d2::gl3_detail::is_current_context_owner(glfwGetCurrentContext()))
        return false;

    const auto current_thread = std::this_thread::get_id();
    if (owner_thread_ == std::thread::id{})
        owner_thread_ = current_thread;
    if (owner_thread_ != current_thread)
        return false;

    GLuint candidate = 0;
    glGenTextures(1, &candidate);
    if (!candidate)
        return false;

    glBindTexture(GL_TEXTURE_2D, candidate);
    const GLint filter = pixelated ? GL_NEAREST : GL_LINEAR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba_data);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (texture_id_)
        glDeleteTextures(1, &texture_id_);

    texture_id_ = candidate;
    width_ = w;
    height_ = h;
    loaded_ = true;
    return true;
}

bool Texture::load(const char* path, bool pixelated) {
    if (!path || !*path)
        return false;

    stbi_set_flip_vertically_on_load(1);

    int w = 0;
    int h = 0;
    int channels = 0;
    unsigned char* data = stbi_load(path, &w, &h, &channels, 4);
    if (!data) {
        std::cerr << "Texture load failed (" << path << "): "
                  << (stbi_failure_reason() ? stbi_failure_reason() : "unknown error") << '\n';
        return false;
    }

    const bool ok = upload_rgba(data, w, h, pixelated);
    stbi_image_free(data);
    return ok;
}

bool Texture::load_from_memory(std::span<const std::uint8_t> rgba_pixels, int width, int height, bool pixelated) {
    if (width <= 0 || height <= 0)
        return false;
    const std::size_t expected_bytes = static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4;
    if (rgba_pixels.size() < expected_bytes)
        return false;

    return upload_rgba(rgba_pixels.data(), width, height, pixelated);
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
