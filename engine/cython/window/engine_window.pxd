from libc.stdint cimport uintptr_t

cdef extern from "engine_window.hpp":
    cdef struct EngineWindowConfig:
        int width
        int height
        const char* title

    int engine_window_init(EngineWindowConfig cfg)
    void engine_window_run() nogil
    void engine_window_shutdown() nogil
    void* engine_window_get_native_handle() nogil
