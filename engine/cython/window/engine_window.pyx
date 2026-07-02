from .engine_window cimport EngineWindowConfig, engine_window_init, engine_window_run, engine_window_shutdown

cdef class EngineWindow:
    cdef EngineWindowConfig cfg
    cdef bytes _title_bytes

    def __cinit__(self, int width, int height, title):
        self.cfg.width = width
        self.cfg.height = height
        self._title_bytes = title.encode("utf-8")
        self.cfg.title = self._title_bytes

    def init(self):
        if not engine_window_init(self.cfg):
            raise RuntimeError("engine_window_init failed")

    def run(self):
        engine_window_run()

    def shutdown(self):
        engine_window_shutdown()
