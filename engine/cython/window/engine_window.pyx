cdef class EngineWindow:
    cdef bytes _title_buf
    cdef EngineWindowConfig cfg

    def __cinit__(self, int w, int h, str title):
        self._title_buf = title.encode("utf-8")
        self.cfg.width = w
        self.cfg.height = h
        self.cfg.title = self._title_buf

        engine_window_init(self.cfg)

    def run(self):
        with nogil:
            engine_window_run()

    def shutdown(self):
        with nogil:
            engine_window_shutdown()

    def get_native_handle(self):
        cdef void* ptr
        with nogil:
            ptr = engine_window_get_native_handle()

        return <uintptr_t>ptr
