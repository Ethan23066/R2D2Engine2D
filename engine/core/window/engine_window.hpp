#pragma once
#include <GLFW/glfw3.h>

struct EngineWindowConfig {
    int width;
    int height;
    const char* title;
};

struct EngineMonitorInfo {
    int monitor_count;
    const char** monitor_names;
};

int engine_window_init(EngineWindowConfig cfg, int monitor_index);
void engine_window_run();
void engine_window_shutdown();
void* engine_window_get_native_handle();

EngineMonitorInfo engine_window_list_monitors();
void engine_window_set_monitor(int index);
