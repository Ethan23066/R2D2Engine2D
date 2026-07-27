#include "engine_window.hpp"
#include <cstdlib>

static GLFWwindow* g_window = nullptr;
static GLFWmonitor* g_selected_monitor = nullptr;
static EngineMonitorInfo g_monitor_info = {0, nullptr};

EngineMonitorInfo engine_window_list_monitors()
{
    if (!glfwInit())
        return {0, nullptr};

    int count = 0;
    GLFWmonitor** monitors = glfwGetMonitors(&count);
    if (!monitors || count == 0)
        return {0, nullptr};

    const char** names = (const char**)std::malloc(sizeof(const char*) * count);
    for (int i = 0; i < count; ++i)
        names[i] = glfwGetMonitorName(monitors[i]);

    g_monitor_info.monitor_count = count;
    g_monitor_info.monitor_names = names;
    return g_monitor_info;
}

void engine_window_set_monitor(int index)
{
    int count = 0;
    GLFWmonitor** monitors = glfwGetMonitors(&count);
    if (!monitors || index < 0 || index >= count)
        return;

    g_selected_monitor = monitors[index];
}

int engine_window_init(EngineWindowConfig cfg, int monitor_index)
{
    if (!glfwInit())
        return 0;

    engine_window_set_monitor(monitor_index);

    g_window = glfwCreateWindow(
        cfg.width,
        cfg.height,
        cfg.title,
        nullptr, // fenêtré ; pour fullscreen: g_selected_monitor
        nullptr
    );

    if (!g_window)
    {
        glfwTerminate();
        return 0;
    }

    glfwMakeContextCurrent(g_window);
    return 1;
}

void engine_window_run()
{
    while (!glfwWindowShouldClose(g_window))
    {
        glfwPollEvents();

        // rendu ici plus tard
        glfwSwapBuffers(g_window);
    }
}

void engine_window_shutdown()
{
    if (g_window)
    {
        glfwDestroyWindow(g_window);
        g_window = nullptr;
    }

    glfwTerminate();

    if (g_monitor_info.monitor_names)
    {
        std::free(g_monitor_info.monitor_names);
        g_monitor_info.monitor_names = nullptr;
        g_monitor_info.monitor_count = 0;
    }
}

void* engine_window_get_native_handle()
{
    return (void*)g_window;
}
