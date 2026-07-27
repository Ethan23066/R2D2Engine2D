#pragma once
#include "engine/core/backend/gl3/RendererGL3.hpp"

class RendererManager {
public:
    RendererManager();

    // nouvelle signature cohérente avec PyGLFW + GL3
    bool init(void* window_ptr, int w, int h);

    void begin_frame(float r, float g, float b, float a);
    void end_frame();
    void shutdown();

private:
    RendererGL3* backend;
};
