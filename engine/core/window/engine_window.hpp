#pragma once

struct EngineWindowConfig {
    int width;
    int height;
    const char* title;
};

int engine_window_init(EngineWindowConfig cfg);
void engine_window_run();
void engine_window_shutdown();

// nouveau : handle natif (GLFWwindow*, X11 Window, etc.)
void* engine_window_get_native_handle();
