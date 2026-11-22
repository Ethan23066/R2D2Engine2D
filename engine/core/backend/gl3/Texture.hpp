#pragma once

#include <cstdint>
#include <span>
#include <thread>

struct Texture {
    Texture() = default;
    ~Texture();
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    bool load(const char* path, bool pixelated = true);
    bool load_from_memory(std::span<const std::uint8_t> rgba_pixels, int width, int height, bool pixelated = true);
    void bind(int unit);

    void set_context_owner(std::thread::id owner) noexcept;
    bool owns_context() const noexcept;
    int width() const noexcept { return width_; }
    int height() const noexcept { return height_; }
    bool is_loaded() const noexcept { return loaded_; }

private:
    bool upload_rgba(const std::uint8_t* rgba_data, int w, int h, bool pixelated);

    std::thread::id owner_thread_{};
    unsigned int texture_id_{0};
    int width_{0};
    int height_{0};
    bool loaded_{false};
};
