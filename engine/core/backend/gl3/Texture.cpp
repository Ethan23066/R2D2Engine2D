#include "Texture.hpp"
#include <glad/glad.h>
#include <thread>
Texture::~Texture() {
    if (texture_id_ && owns_context()) glDeleteTextures(1, &texture_id_);
}
void Texture::set_context_owner(std::thread::id owner) noexcept { owner_thread_ = owner; }
bool Texture::owns_context() const noexcept {
    return owner_thread_ == std::thread::id{} || owner_thread_ == std::this_thread::get_id();
}
bool Texture::load(const char* path) {
    if (!owns_context() || !path || !*path) return false;
    // No image decoder is currently part of this backend. Returning false is
    // preferable to reporting success for an empty, uninitialized GPU texture.
    return false;
}
void Texture::bind(int unit) {
    if (!owns_context() || !loaded_ || !texture_id_) return;
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
}
