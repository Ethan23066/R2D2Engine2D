#include "Texture.hpp"
#include <glad/glad.h>
#include <thread>
Texture::~Texture() {
    // Destroy GPU objects while the owning context is still alive.
    if (texture_id_ && owns_context()) glDeleteTextures(1, &texture_id_);
}
void Texture::set_context_owner(std::thread::id owner) noexcept { owner_thread_ = owner; }
bool Texture::owns_context() const noexcept {
    return owner_thread_ == std::thread::id{} || owner_thread_ == std::this_thread::get_id();
}
bool Texture::load(const char* path) {
    if (!owns_context() || !path || !*path) return false;
    // Image decoding is not wired into this backend yet. Do not claim that a
    // file was loaded or create an empty texture that looks valid to callers.
    return false;
}
void Texture::bind(int unit) {
    if (!owns_context() || !loaded_ || !texture_id_) return;
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
}
