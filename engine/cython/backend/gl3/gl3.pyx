# cython: language_level=3

from engine.cython.backend.gl3.gl3 cimport RendererGL3, MeshGL3, ShaderGL3, Texture

# ---------------- Renderer ----------------

cdef class PyRendererGL3:
    cdef RendererGL3* ptr

    def __cinit__(self):
        self.ptr = new RendererGL3()   # OK : constructeur C++

    def __dealloc__(self):
        if self.ptr is not NULL:
            del self.ptr               # OK : destruction C++

    def init(self, window_ptr: int, w: int, h: int):
        cdef void* win = <void*> window_ptr   # OK : cast Python → void*
        return self.ptr.init(win, w, h)       # ❗ PROBLÈME SI init() RETOURNE bool

    def begin_frame(self, r: float, g: float, b: float, a: float):
        self.ptr.begin_frame(r, g, b, a)      # OK

    def end_frame(self):
        self.ptr.end_frame()                  # OK

    def shutdown(self):
        self.ptr.shutdown()                   # OK
