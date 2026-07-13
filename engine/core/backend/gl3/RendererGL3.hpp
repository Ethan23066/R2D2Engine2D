#pragma once
#include "engine/core/renderer/IRenderer.hpp"
#include <GL/gl.h>

class RendererGL3 : public IRenderer {
public:
    bool init() override;
    void resize(int w, int h) override;
    void begin_frame() override;
    void end_frame() override;
    void shutdown() override;
};
