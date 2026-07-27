from engine.cython.backend.gl3.gl3 import PyRendererGL3

class BackendManager:
    def __init__(self):
        self.renderer = PyRendererGL3()

    def init(self, window_ptr: int, w: int, h: int):
        return self.renderer.init(window_ptr, w, h)

    def begin_frame(self, r=0.1, g=0.1, b=0.1, a=1.0):
        self.renderer.begin_frame(r, g, b, a)

    def end_frame(self):
        self.renderer.end_frame()

    def shutdown(self):
        self.renderer.shutdown()
