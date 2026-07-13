from engine.cython.backend.gl3.gl3 cimport RendererGL3

cdef class PyRendererGL3:
    cdef RendererGL3* ptr

    def __cinit__(self):
        self.ptr = new RendererGL3()

    def init(self):
        return self.ptr.init()

    def resize(self, int w, int h):
        self.ptr.resize(w, h)

    def begin_frame(self):
        self.ptr.begin_frame()

    def end_frame(self):
        self.ptr.end_frame()

    def shutdown(self):
        self.ptr.shutdown()
