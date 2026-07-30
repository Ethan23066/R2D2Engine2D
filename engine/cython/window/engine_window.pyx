# distutils: language = c++

import glfw
from libc.stdlib cimport malloc, free
from libc.string cimport strdup
from libc.stdint cimport uintptr_t

from .engine_window cimport (
    EngineMonitorMode,
    EngineWindowConfig,
    engine_window_select_monitor,
    engine_window_init,
    engine_window_run,
    engine_window_shutdown,
    engine_window_get_native_handle
)


# ============================================================
#  CLASSE ENGINEWINDOW (Python → Cython → C++)
# ============================================================

cdef class EngineWindow:
    cdef EngineWindowConfig cfg
    cdef int monitor_index

    # --------------------------------------------------------
    #  Constructeur
    # --------------------------------------------------------
    def __cinit__(self, int width, int height, str title, int monitor_index=0):
        self.cfg.width = width
        self.cfg.height = height

        # Chaîne C sécurisée
        self.cfg.title = strdup(title.encode("utf-8"))

        self.monitor_index = monitor_index
        engine_window_init(self.cfg, self.monitor_index)

    # --------------------------------------------------------
    #  Méthodes statiques exposées à Python
    # --------------------------------------------------------

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

    # --------------------------------------------------------
    #  Méthodes d’instance
    # --------------------------------------------------------

    def run(self):
        engine_window_run()

    def shutdown(self):
        free(<void*> self.cfg.title)
        engine_window_shutdown()

    def get_native_handle(self):
        return <uintptr_t> engine_window_get_native_handle()
