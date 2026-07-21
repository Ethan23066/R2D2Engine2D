#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct RendererGL3;

class RendererManager {
public:
    RendererManager();
    ~RendererManager();

    bool init(GLFWwindow* window, int w, int h);
    void begin_frame();
    void end_frame();
    void shutdown();

private:
    RendererGL3* backend;
};
