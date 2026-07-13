#include "RendererGL3.hpp"
#include <GL/gl.h>

RendererGL3::RendererGL3() {}

bool RendererGL3::init() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    return true;
}

void RendererGL3::resize(int w, int h) {
    glViewport(0, 0, w, h);
}

void RendererGL3::begin_frame() {
    glClear(GL_COLOR_BUFFER_BIT);
}

void RendererGL3::end_frame() {
    // rien pour l'instant
}

void RendererGL3::shutdown() {
    // rien pour l’instant
}
