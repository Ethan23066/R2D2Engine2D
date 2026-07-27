#include "RendererGL3.hpp"
#include <iostream>

RendererGL3::RendererGL3() {}

int RendererGL3::init(void* win, int w, int h) {
    this->window = win;
    this->width = w;
    this->height = h;

    // PyGLFW a déjà créé le contexte OpenGL
    if (!gladLoadGL()) {
        std::cerr << "Failed to load GL via glad\n";
        return 0;
    }

    glViewport(0, 0, w, h);
    return 1;
}

void RendererGL3::begin_frame(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void RendererGL3::end_frame() {
    // Le swap est fait en Python via PyGLFW
}

void RendererGL3::shutdown() {
    window = nullptr;
}
