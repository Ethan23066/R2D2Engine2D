import glfw
from engine.cython.window.engine_window cimport EngineWindowConfig, engine_window_init

cdef class EngineWindow:
    cdef EngineWindowConfig cfg
    cdef object py_window
    cdef bytes _title_bytes  # on garde les bytes ici

    def __cinit__(self, int width, int height, title):
        self.cfg.width = width
        self.cfg.height = height
        self._title_bytes = title.encode("utf-8")
        self.cfg.title = self._title_bytes  # pointeur vers les bytes, qui restent vivants

    def init(self):
        engine_window_init(self.cfg)

        if not glfw.init():
            raise RuntimeError("Failed to init PyGLFW")

        self.py_window = glfw.create_window(
            self.cfg.width,
            self.cfg.height,
            self._title_bytes.decode("utf-8"),
            None,
            None
        )

        if not self.py_window:
            raise RuntimeError("Failed to create PyGLFW window")

        glfw.make_context_current(self.py_window)

    def ptr(self):
        return <unsigned long> id(self.py_window)
