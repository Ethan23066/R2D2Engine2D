#pragma once
#include "glad/glad.h"
#include <GLFW/glfw3.h>

struct RendererGL3 {
    GLFWwindow* window = nullptr;
    int width = 0;
    int height = 0;

    bool init(GLFWwindow* win, int w, int h);
    void begin_frame(float r, float g, float b, float a);
    void end_frame();
    void shutdown();
};
