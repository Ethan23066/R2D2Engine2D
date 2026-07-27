cdef extern from "engine_window.hpp":
    cdef struct EngineWindowConfig:
        int width
        int height
        const char* title

    bool engine_window_init(EngineWindowConfig cfg)   # ← PAS nogil
    void engine_window_run() nogil
    void engine_window_shutdown() nogil
