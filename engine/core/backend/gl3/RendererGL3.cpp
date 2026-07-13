#include "RendererGL3.hpp"
#include <cstdio>

bool RendererGL3::init() {
    printf("[GL3] Init\n");
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f, 1.0f);
    return true;
}

void RendererGL3::resize(int w, int h) {
    glViewport(0, 0, w, h);
}

void RendererGL3::begin_frame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RendererGL3::end_frame() {
    // le swap se fait côté window backend
}

void RendererGL3::shutdown() {
    printf("[GL3] Shutdown\n");
}
