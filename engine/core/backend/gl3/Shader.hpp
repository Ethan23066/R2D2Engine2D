#pragma once
#include <string>
#include <GL/gl.h>

class Shader {
public:
    Shader();
    ~Shader();

    bool load(const std::string& vs_src, const std::string& fs_src);
    void use() const;
    GLint uniform_location(const char* name) const;

private:
    GLuint program = 0;
    GLuint vs = 0;
    GLuint fs = 0;

    bool compile(GLuint& out, GLenum type, const std::string& src);
};
