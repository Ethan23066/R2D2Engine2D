#include "Texture.hpp"

#include <glad/glad.h>
#include <thread>

Texture::Texture() = default;

Texture::~Texture() = default;

void Texture::set_context_owner(std::thread::id owner) noexcept {
    std::scoped_lock lock(gl_mutex_);
    owner_thread_ = owner;
}

bool Texture::owns_context() const noexcept {
    const auto current = std::this_thread::get_id();
    std::scoped_lock lock(gl_mutex_);
    return owner_thread_ == std::thread::id{} || owner_thread_ == current;
}

bool Texture::load(const char* path)
{
    if (!owns_context())
        return false;

    std::scoped_lock lock(gl_mutex_);
    (void)path;

    glGenTextures(1, &texture_id_);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    loaded_ = true;
    return true;
}

void Texture::bind(int unit)
{
    if (!owns_context() || !loaded_)
        return;

    std::scoped_lock lock(gl_mutex_);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
}
