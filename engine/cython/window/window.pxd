# distutils: language = c++

cdef extern from "window.hpp":
    cdef struct EngineMonitorMode:
        int width
        int height
        int refresh

    cdef struct EngineWindowConfig:
        int width
        int height
        const char* title

    int engine_window_select_monitor(const EngineMonitorMode* modes, int count)
    int engine_window_init(const EngineWindowConfig* cfg, int monitor_index)
    void engine_window_run()
    void engine_window_shutdown()
    void* engine_window_get_native_handle()
