#pragma once

struct ShaderGL3 {
    ShaderGL3();

    bool load(const char* vs_path, const char* fs_path);
    void use();
};
