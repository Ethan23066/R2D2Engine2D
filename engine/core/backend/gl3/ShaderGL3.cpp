#include "ShaderGL3.hpp"
#include <vector>

static GLuint compile_stage(GLenum type, const std::string& src) {
    GLuint shader = glCreateShader(type);
    const char* csrc = src.c_str();
    glShaderSource(shader, 1, &csrc, nullptr);
    glCompileShader(shader);

    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool ShaderGL3::load(const std::string& vs_src, const std::string& fs_src) {
    GLuint vs = compile_stage(GL_VERTEX_SHADER, vs_src);
    if (!vs) return false;

    GLuint fs = compile_stage(GL_FRAGMENT_SHADER, fs_src);
    if (!fs) {
        glDeleteShader(vs);
        return false;
    }

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint ok = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok) {
        glDeleteProgram(program);
        program = 0;
        return false;
    }

    return true;
}

void ShaderGL3::bind() const {
    glUseProgram(program);
}

void ShaderGL3::destroy() {
    if (program) {
        glDeleteProgram(program);
        program = 0;
    }
}

GLint ShaderGL3::get_uniform(const char* name) const {
    return glGetUniformLocation(program, name);
}
