#include "ShaderGL3.hpp"

#include <glad/glad.h>
#include <thread>

ShaderGL3::ShaderGL3() = default;

ShaderGL3::~ShaderGL3() = default;

void ShaderGL3::set_context_owner(std::thread::id owner) noexcept {
    std::scoped_lock lock(gl_mutex_);
    owner_thread_ = owner;
}

bool ShaderGL3::owns_context() const noexcept {
    const auto current = std::this_thread::get_id();
    std::scoped_lock lock(gl_mutex_);
    return owner_thread_ == std::thread::id{} || owner_thread_ == current;
}

bool ShaderGL3::load(const char* vs_path, const char* fs_path)
{
    if (!owns_context())
        return false;

    std::scoped_lock lock(gl_mutex_);
    (void)vs_path;
    (void)fs_path;

    const unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    const unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    program_ = glCreateProgram();

    glShaderSource(vs, 1, nullptr, nullptr);
    glCompileShader(vs);
    glAttachShader(program_, vs);

    glShaderSource(fs, 1, nullptr, nullptr);
    glCompileShader(fs);
    glAttachShader(program_, fs);

    glLinkProgram(program_);
    glDeleteShader(vs);
    glDeleteShader(fs);

    compiled_ = true;
    return true;
}

void ShaderGL3::use()
{
    if (!owns_context() || !compiled_)
        return;

    std::scoped_lock lock(gl_mutex_);
    glUseProgram(program_);
}
