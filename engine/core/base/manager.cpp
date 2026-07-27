#include "manager.hpp"

RendererManager::RendererManager() {
    backend = new RendererGL3();
}

bool RendererManager::init(void* window_ptr, int w, int h) {
    return backend->init(window_ptr, w, h);
}

void RendererManager::begin_frame(float r, float g, float b, float a) {
    backend->begin_frame(r, g, b, a);
}

void RendererManager::end_frame() {
    backend->end_frame();
}

void RendererManager::shutdown() {
    backend->shutdown();
}
