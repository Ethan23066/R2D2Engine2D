#include "RendererGL3.hpp"

RendererGL3::RendererGL3() {}

int RendererGL3::init(void* win, int w, int h) {
    this->window = win;
    this->width = w;
    this->height = h;
    return true;
}

void RendererGL3::begin_frame(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void RendererGL3::end_frame() {
    // GLFWwindow* glfw_win = static_cast<GLFWwindow*>(window);
    // glfwSwapBuffers(glfw_win);
}

void RendererGL3::shutdown() {
    window = nullptr;
}
