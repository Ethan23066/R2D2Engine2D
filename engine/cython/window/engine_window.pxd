cdef extern from "engine_window.hpp":
    cdef struct EngineWindowConfig:
        int width
        int height
        const char* title

    cdef struct EngineMonitorInfo:
        int monitor_count
        const char** monitor_names

    EngineMonitorInfo engine_window_list_monitors() nogil
    void engine_window_set_monitor(int index) nogil
    int engine_window_init(EngineWindowConfig cfg, int monitor_index) nogil
    void engine_window_run() nogil
    void engine_window_shutdown() nogil
    void* engine_window_get_native_handle() nogil
