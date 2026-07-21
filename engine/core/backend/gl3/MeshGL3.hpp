#pragma once
#include "glad/glad.h"

struct MeshGL3 {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLsizei index_count = 0;

    bool init(const float* vertices, GLsizei vertex_count,
              const unsigned int* indices, GLsizei index_count);
    void draw() const;
    void destroy();
};
