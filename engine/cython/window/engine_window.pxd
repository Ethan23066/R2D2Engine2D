cdef extern from "engine_window.hpp":
    cdef struct EngineWindowConfig:
        int width
        int height
        const char* title

    bint engine_window_init(EngineWindowConfig cfg)
    void engine_window_run()
    void engine_window_shutdown()
