# engine/cython/backend/manager.pyx

from engine.cython.backend.gl3.gl3 import PyRendererGL3


class BackendManager:
    def __init__(self):
        self.renderer = PyRendererGL3()

    def init(self, window_ptr: int, w: int, h: int):
        return self.renderer.init(window_ptr, w, h)

    def begin_frame(self, r: float = 0.1, g: float = 0.1, b: float = 0.1, a: float = 1.0):
        self.renderer.begin_frame(r, g, b, a)

    def end_frame(self):
        self.renderer.end_frame()

    def shutdown(self):
        self.renderer.shutdown()
