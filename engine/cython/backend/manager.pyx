from engine.cython.backend.gl3.gl3 cimport PyRendererGL3

cdef class BackendManager:
    def __cinit__(self):
        self.renderer = PyRendererGL3()

    def init(self, long window_ptr, int w, int h):
        return self.renderer.init(window_ptr, w, h)

    def begin_frame(self, float r=0.1, float g=0.1, float b=0.1, float a=1.0):
        self.renderer.begin_frame(r, g, b, a)

    def end_frame(self):
        self.renderer.end_frame()

    def shutdown(self):
        self.renderer.shutdown()
