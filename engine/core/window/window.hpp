#pragma once

struct EngineMonitorMode {
    int width;
    int height;
    int refresh;
};

struct EngineWindowConfig {
    int width;
    int height;
    const char* title;
};

int engine_window_select_monitor(const EngineMonitorMode* modes, int count);

// Ces fonctions ne gèrent PAS la fenêtre : elles reçoivent un handle Python
int engine_window_init(const EngineWindowConfig* cfg, int monitor_index);
void engine_window_run();
void engine_window_shutdown();
void* engine_window_get_native_handle();
