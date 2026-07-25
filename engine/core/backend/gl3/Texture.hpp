#pragma once

struct Texture {
    Texture();

    bool load(const char* path);
    void bind(int unit);
};
