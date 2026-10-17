#include "window.hpp"

#include <GLFW/glfw3.h>
#include <iostream>

static GLFWwindow* g_window = nullptr;

int engine_window_select_monitor(const EngineMonitorMode* modes, int count)
{
    if (count <= 0)
        return 0;

    if (count == 2)
        return 1;

    int best_index = 0;
    int best_area = modes[0].width * modes[0].height;

    for (int i = 1; i < count; ++i) {
        int area = modes[i].width * modes[i].height;
        if (area > best_area) {
            best_area = area;
            best_index = i;
        }
    }
