#include "RendererGL3.hpp"

bool RendererGL3::init(GLFWwindow* win, int w, int h) {
    window = win;
    width = w;
    height = h;

    if (!gladLoadGL()) {
        return false;
    }

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    return true;
}

void RendererGL3::begin_frame(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RendererGL3::end_frame() {
    glfwSwapBuffers(window);
}

void RendererGL3::shutdown() {
    window = nullptr;
}
