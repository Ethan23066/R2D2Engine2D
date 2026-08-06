#include "window.hpp"

// Handle fourni par Python (pyGLFW)
static void* g_native_handle = nullptr;

int engine_window_select_monitor(const EngineMonitorMode* modes, int count)
{
    if (count <= 0)
        return 0;

    if (count == 2)
        return 1;

    int best_index = 1;
    int best_area = modes[1].width * modes[1].height;

    for (int i = 2; i < count; ++i) {
        int area = modes[i].width * modes[i].height;
        if (area > best_area) {
            best_area = area;
            best_index = i;
        }
    }

    return best_index;
}

// Python fournit le handle via Cython
int engine_window_init(const EngineWindowConfig* cfg, int monitor_index)
{
    (void)cfg;
    (void)monitor_index;
    return 1; // rien à faire en C++
}

void engine_window_run()
{
    // La boucle est gérée en Python (pyGLFW)
}

void engine_window_shutdown()
{
    g_native_handle = nullptr;
}

void* engine_window_get_native_handle()
{
    return g_native_handle;
}
