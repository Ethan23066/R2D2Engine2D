# cython: language_level=3

from engine.cython.backend.gl3.gl3 cimport RendererGL3, MeshGL3, ShaderGL3, Texture

# ---------------- Renderer ----------------

cdef class PyRendererGL3:
    cdef RendererGL3* ptr

    def __cinit__(self):
        self.ptr = new RendererGL3()

    def __dealloc__(self):
        if self.ptr is not NULL:
            del self.ptr

    def init(self, window_ptr: int, w: int, h: int):
        cdef void* win = <void*> window_ptr
        return self.ptr.init(win, w, h)

    def begin_frame(self, r: float, g: float, b: float, a: float):
        self.ptr.begin_frame(r, g, b, a)

    def end_frame(self):
        self.ptr.end_frame()

    def shutdown(self):
        self.ptr.shutdown()


# ---------------- Mesh ----------------

cdef class PyMeshGL3:
    cdef MeshGL3* ptr

    def __cinit__(self):
        self.ptr = new MeshGL3()

    def __dealloc__(self):
        if self.ptr is not NULL:
            del self.ptr

    def upload(self):
        self.ptr.upload()

    def draw(self):
        self.ptr.draw()


# ---------------- Shader ----------------

cdef class PyShaderGL3:
    cdef ShaderGL3* ptr

    def __cinit__(self):
        self.ptr = new ShaderGL3()

    def __dealloc__(self):
        if self.ptr is not NULL:
            del self.ptr

    def load(self, vs_path: str, fs_path: str):
        cdef bytes vs_b = vs_path.encode("utf-8")
        cdef bytes fs_b = fs_path.encode("utf-8")
        return self.ptr.load(vs_b, fs_b)

    def use(self):
        self.ptr.use()


# ---------------- Texture ----------------

cdef class PyTexture:
    cdef Texture* ptr

    def __cinit__(self):
        self.ptr = new Texture()

    def __dealloc__(self):
        if self.ptr is not NULL:
            del self.ptr

    def load(self, path: str):
        cdef bytes p = path.encode("utf-8")
        return self.ptr.load(p)

    def bind(self, unit: int):
        self.ptr.bind(unit)
