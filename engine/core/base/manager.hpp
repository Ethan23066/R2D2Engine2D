#pragma once

#include "log.hpp"
#include "engine/core/backend/gl3/RendererGL3.hpp"

class RendererManager {
public:
    RendererManager();
    ~RendererManager();

    bool init();
    void resize(int w, int h);
    void begin_frame();
    void end_frame();
    void shutdown();

private:
    RendererGL3* backend;
};
