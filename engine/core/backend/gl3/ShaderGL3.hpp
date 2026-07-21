#pragma once
#include "glad/glad.h"
#include <string>

struct ShaderGL3 {
    GLuint program = 0;

    bool load(const std::string& vs_src, const std::string& fs_src);
    void bind() const;
    void destroy();

    GLint get_uniform(const char* name) const;
};
