#pragma once
#include "glad/glad.h"

struct RendererGL3 {
    void* window = nullptr;
    int width = 0;
    int height = 0;

    RendererGL3();

    int init(void* win, int w, int h);
    void begin_frame(float r, float g, float b, float a);
    void end_frame();
    void shutdown();
};
