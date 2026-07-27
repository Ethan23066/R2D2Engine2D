from engine.cython.backend.gl3.gl3 cimport RendererGL3

cdef class PyRendererGL3:
    cdef RendererGL3* _r

    def __cinit__(self):
        self._r = new RendererGL3()

    def init(self, unsigned long window_ptr, int w, int h):
        return self._r.init(<void*>window_ptr, w, h)

    def begin_frame(self, float r, float g, float b, float a):
        self._r.begin_frame(r, g, b, a)

    def end_frame(self):
        self._r.end_frame()

    def shutdown(self):
        self._r.shutdown()
