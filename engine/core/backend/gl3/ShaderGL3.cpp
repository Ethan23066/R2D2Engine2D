#include "ShaderGL3.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "GLContext.hpp"

#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>
#include <thread>

namespace {
bool read_text_file(const char* path, std::string& output) {
    if (!path || !*path)
        return false;

    std::ifstream file(path, std::ios::binary);
    if (!file)
        return false;

    output.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return file.good() || file.eof();
}

bool compile_shader(GLuint shader, const std::string& source, const char* label) {
    if (source.size() > static_cast<std::size_t>(std::numeric_limits<GLint>::max())) {
        std::cerr << "Shader source is too large: " << label << '\n';
        return false;
    }

    const char* source_ptr = source.c_str();
    const GLint length = static_cast<GLint>(source.size());
    glShaderSource(shader, 1, &source_ptr, &length);
    glCompileShader(shader);

    GLint status = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_TRUE)
        return true;

    GLint log_length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
    std::string log(static_cast<std::size_t>(log_length > 1 ? log_length : 1), '\0');
    glGetShaderInfoLog(shader, log_length, nullptr, log.data());
    std::cerr << "Shader compile failed (" << label << "): " << log << '\n';
    return false;
}
}

bool ShaderGL3::owns_context() const noexcept {
    return owner_thread_ == std::this_thread::get_id()
        && r2d2::gl3_detail::is_current_context_owner(glfwGetCurrentContext());
}

ShaderGL3::~ShaderGL3() {
    // GL resources must be released while their owning context is current.
    if (program_ && owns_context())
        glDeleteProgram(program_);
}

void ShaderGL3::set_context_owner(std::thread::id owner) noexcept {
    owner_thread_ = owner;
}

bool ShaderGL3::load(const char* vs_path, const char* fs_path) {
    if (!r2d2::gl3_detail::is_current_context_owner(glfwGetCurrentContext()))
        return false;

    const auto current_thread = std::this_thread::get_id();
    if (owner_thread_ == std::thread::id{})
        owner_thread_ = current_thread;
    if (owner_thread_ != current_thread)
        return false;

    std::string vs_source;
    std::string fs_source;
    if (!read_text_file(vs_path, vs_source) || !read_text_file(fs_path, fs_source)) {
        std::cerr << "Unable to read shader source files\n";
        return false;
    }

    const GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    if (!vs || !fs) {
        if (vs) glDeleteShader(vs);
        if (fs) glDeleteShader(fs);
        return false;
    }

    const bool vertex_ok = compile_shader(vs, vs_source, vs_path);
    const bool fragment_ok = compile_shader(fs, fs_source, fs_path);
    if (!vertex_ok || !fragment_ok) {
        glDeleteShader(vs);
        glDeleteShader(fs);
        return false;
    }

    const GLuint candidate = glCreateProgram();
    if (!candidate) {
        glDeleteShader(vs);
        glDeleteShader(fs);
        return false;
    }

    glAttachShader(candidate, vs);
    glAttachShader(candidate, fs);
    glLinkProgram(candidate);
    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint linked = GL_FALSE;
    glGetProgramiv(candidate, GL_LINK_STATUS, &linked);
    if (linked != GL_TRUE) {
        GLint log_length = 0;
        glGetProgramiv(candidate, GL_INFO_LOG_LENGTH, &log_length);
        std::string log(static_cast<std::size_t>(log_length > 1 ? log_length : 1), '\0');
        glGetProgramInfoLog(candidate, log_length, nullptr, log.data());
        std::cerr << "Shader link failed: " << log << '\n';
        glDeleteProgram(candidate);
        return false;
    }

    if (program_)
        glDeleteProgram(program_);

    program_ = candidate;
    compiled_ = true;
    return true;
}

void ShaderGL3::use() {
    if (owns_context() && compiled_ && program_)
        glUseProgram(program_);
}
void ShaderGL3::set_int(const char* name, int value) {
    if (!name || !owns_context() || !compiled_ || !program_) return;
    const GLint loc = glGetUniformLocation(program_, name);
    if (loc != -1) glUniform1i(loc, value);
}

void ShaderGL3::set_float(const char* name, float value) {
    if (!name || !owns_context() || !compiled_ || !program_) return;
    const GLint loc = glGetUniformLocation(program_, name);
    if (loc != -1) glUniform1f(loc, value);
}

void ShaderGL3::set_vec2(const char* name, float x, float y) {
    if (!name || !owns_context() || !compiled_ || !program_) return;
    const GLint loc = glGetUniformLocation(program_, name);
    if (loc != -1) glUniform2f(loc, x, y);
}

void ShaderGL3::set_vec4(const char* name, float x, float y, float z, float w) {
    if (!name || !owns_context() || !compiled_ || !program_) return;
    const GLint loc = glGetUniformLocation(program_, name);
    if (loc != -1) glUniform4f(loc, x, y, z, w);
}
