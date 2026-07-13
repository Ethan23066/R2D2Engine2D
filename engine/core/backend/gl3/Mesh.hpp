#pragma once
#include <GL/glad.h>

class Mesh {
public:
    Mesh();
    ~Mesh();

    bool create(const float* vertices, int vcount,
                const unsigned int* indices, int icount);

    void draw() const;

private:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLsizei index_count = 0;
};
