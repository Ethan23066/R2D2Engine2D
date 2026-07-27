#pragma once
#include "engine/core/window/engine_window.hpp"
#include "engine/core/backend/gl3/RendererGL3.hpp"

class RendererManager {
public:
    RendererManager();
    ~RendererManager();

    bool init(const EngineWindowConfig& cfg);
    void begin_frame();
    void end_frame();
    void shutdown();

private:
    RendererGL3* backend;
};
