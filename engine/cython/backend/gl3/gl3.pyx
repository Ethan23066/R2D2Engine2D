from engine.cython.backend.gl3.gl3 cimport RendererGL3, MeshGL3, ShaderGL3, GLFWwindow
from libcpp.string cimport string
from libc.stdlib cimport malloc, free

cdef class PyRendererGL3:
    cdef RendererGL3* ptr

    def __cinit__(self):
        self.ptr = new RendererGL3()

    def init(self, window_ptr: int, w: int, h: int):
        cdef GLFWwindow* win = <GLFWwindow*>window_ptr
        return self.ptr.init(win, w, h)
