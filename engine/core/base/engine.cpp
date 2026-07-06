// engine/core/base/engine.cpp
#include "engine.hpp"

namespace r2d2 {

    Engine::Engine() : running(false) {}

    Engine::~Engine() {
        if (running) shutdown();
    }

    void Engine::init() {
        running = true;
    }

    void Engine::shutdown() {
        running = false;
    }

    void Engine::tick() {
        if (!running) return;
        // future: events, lifecycle, renderer, input...
    }

    bool Engine::is_running() const {
        return running;
    }

}
