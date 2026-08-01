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
    int engine_window_init(const EngineWindowConfig& cfg, int monitor_index)
    void engine_window_run()
    void engine_window_shutdown()
    void* engine_window_get_native_handle()

import glfw
from libc.stdlib cimport malloc, free
from libc.string cimport strdup
from libc.stdint cimport uintptr_t

cdef class EngineWindow:
    cdef EngineWindowConfig cfg
    cdef int monitor_index
    cdef object window   # <-- AJOUT

    def __cinit__(self, int width, int height, str title, int monitor_index=0):
        self.cfg.width = width
        self.cfg.height = height
        self.cfg.title = strdup(title.encode("utf-8"))
        self.monitor_index = monitor_index

        # --- Appel C++ existant ---
        engine_window_init(self.cfg, self.monitor_index)

        # --- AJOUT : création de la fenêtre GLFW ---
        if not glfw.init():
            raise RuntimeError("GLFW init failed")

        monitors = glfw.get_monitors()
        monitor = monitors[self.monitor_index]

        self.window = glfw.create_window(width, height, title, monitor, None)
        if not self.window:
            raise RuntimeError("GLFW window creation failed")

        glfw.make_context_current(self.window)

    @staticmethod
    def list_monitors():
        if not glfw.init():
            return []

        monitors = glfw.get_monitors()
        if not monitors:
            return []

        return [glfw.get_monitor_name(m) for m in monitors]

    @staticmethod
    def get_monitor_mode(int index):
        if not glfw.init():
            return (0, 0, 0)

        monitors = glfw.get_monitors()
        if not monitors or index < 0 or index >= len(monitors):
            return (0, 0, 0)

        mode = glfw.get_video_mode(monitors[index])
        return (mode.size.width, mode.size.height, mode.refresh_rate)

    @staticmethod
    def select_monitor(modes):
        cdef int count = len(modes)
        cdef EngineMonitorMode* arr = <EngineMonitorMode*> malloc(count * sizeof(EngineMonitorMode))

        for i in range(count):
            arr[i].width = modes[i][0]
            arr[i].height = modes[i][1]
            arr[i].refresh = modes[i][2]

        idx = engine_window_select_monitor(arr, count)
        free(arr)
        return idx

    def run(self):
        # --- AJOUT : boucle GLFW ---
        while not glfw.window_should_close(self.window):
            glfw.poll_events()
            glfw.swap_buffers(self.window)

        # --- Appel C++ existant ---
        engine_window_run()

    def shutdown(self):
        glfw.destroy_window(self.window)
        glfw.terminate()

        free(<void*> self.cfg.title)
        engine_window_shutdown()

    def get_native_handle(self):
        return <uintptr_t> engine_window_get_native_handle()
