#pragma once
#include <glad/glad.h>

class Texture {
public:
    Texture();
    ~Texture();

    bool create(int w, int h, const unsigned char* data);
    void bind(int unit = 0) const;

private:
    GLuint id = 0;
    int width = 0;
    int height = 0;
};
