#include "manager.hpp"
#include "engine/core/backend/gl3/RendererGL3.hpp"
#include "log.hpp"

RendererManager::RendererManager() {
    backend = new RendererGL3();
    r2d2::Log::info("Backend forcé : GL3");
}

RendererManager::~RendererManager() {
    if (backend) {
        backend->shutdown();
        delete backend;
        backend = nullptr;
    }
}

bool RendererManager::init() {
    return backend->init();
}

void RendererManager::resize(int w, int h) {
    backend->resize(w, h);
}

void RendererManager::begin_frame() {
    backend->begin_frame();
}

void RendererManager::end_frame() {
    backend->end_frame();
}

void RendererManager::shutdown() {
    backend->shutdown();
}
