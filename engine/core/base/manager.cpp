#include "manager.hpp"
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

bool RendererManager::init(const EngineWindowConfig& cfg) {
    return backend->init(cfg.width, cfg.height, cfg.title);
}

void RendererManager::begin_frame() {
    backend->begin_frame(0.1f, 0.1f, 0.1f, 1.0f);
}

void RendererManager::end_frame() {
    backend->end_frame();
}

void RendererManager::shutdown() {
    backend->shutdown();
}
