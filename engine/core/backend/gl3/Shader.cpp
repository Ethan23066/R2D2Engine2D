#include "Shader.hpp"
#include <cstdio>

Shader::Shader() {}
Shader::~Shader() {
    if (program) glDeleteProgram(program);
    if (vs) glDeleteShader(vs);
    if (fs) glDeleteShader(fs);
}

bool Shader::compile(GLuint& out, GLenum type, const std::string& src) {
    out = glCreateShader(type);
    const char* csrc = src.c_str();
    glShaderSource(out, 1, &csrc, nullptr);
    glCompileShader(out);

    GLint ok = 0;
    glGetShaderiv(out, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(out, sizeof(log), nullptr, log);
        std::printf("[Shader] Compile error: %s\n", log);
        return false;
    }
    return true;
}

bool Shader::load(const std::string& vs_src, const std::string& fs_src) {
    if (!compile(vs, GL_VERTEX_SHADER, vs_src)) return false;
    if (!compile(fs, GL_FRAGMENT_SHADER, fs_src)) return false;

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    GLint ok = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        std::printf("[Shader] Link error: %s\n", log);
        return false;
    }
    return true;
}

void Shader::use() const {
    glUseProgram(program);
}

GLint Shader::uniform_location(const char* name) const {
    return glGetUniformLocation(program, name);
}
