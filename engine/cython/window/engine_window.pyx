# cython: language_level=3
from libc.stdint cimport uintptr_t

from .engine_window cimport (
    EngineWindowConfig,
    EngineMonitorInfo,
    engine_window_list_monitors,
    engine_window_set_monitor,
    engine_window_init,
    engine_window_run,
    engine_window_shutdown,
    engine_window_get_native_handle,
)

cdef class EngineWindow:
    cdef EngineWindowConfig cfg
    cdef int monitor_index
    cdef bytes _title_buf

    def __cinit__(self, int width, int height, str title, int monitor_index=0):
        self.cfg.width = width
        self.cfg.height = height

        self._title_buf = title.encode("utf-8")
        self.cfg.title = self._title_buf

        self.monitor_index = monitor_index

        if engine_window_init(self.cfg, self.monitor_index) == 0:
            raise RuntimeError("Failed to init window")

    def run(self):
        engine_window_run()

    def shutdown(self):
        engine_window_shutdown()

    @staticmethod
    def list_monitors():
        cdef EngineMonitorInfo info = engine_window_list_monitors()
        monitors = []
        for i in range(info.monitor_count):
            monitors.append(info.monitor_names[i].decode("utf-8"))
        return monitors

    @staticmethod
    def set_monitor(int index):
        engine_window_set_monitor(index)

    def get_native_handle(self):
        return <uintptr_t>engine_window_get_native_handle()
