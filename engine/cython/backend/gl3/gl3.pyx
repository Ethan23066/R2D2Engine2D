from engine.cython.backend.gl3.gl3 cimport RendererGL3

cdef class PyRendererGL3:
    def __cinit__(self):
        self.cpp_renderer = new RendererGL3()

    def init(self, long window_ptr, int w, int h):
        return self.cpp_renderer.init(<void*>window_ptr, w, h)

    def begin_frame(self, float r, float g, float b, float a):
        self.cpp_renderer.begin_frame(r, g, b, a)

    def end_frame(self):
        self.cpp_renderer.end_frame()

    def shutdown(self):
        self.cpp_renderer.shutdown()
        del self.cpp_renderer
