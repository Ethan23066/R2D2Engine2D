# engine/cython/backend/gl3/gl3.pyx

from libcpp.string cimport string
from libc.stdlib cimport malloc, free

cimport gl3


cdef class PyRendererGL3:
    cdef gl3.RendererGL3* ptr

    def __cinit__(self):
        self.ptr = new gl3.RendererGL3()

    def init(self, window_ptr: int, w: int, h: int):
        return self.ptr.init(<void*>window_ptr, w, h)

    def begin_frame(self, r: float, g: float, b: float, a: float):
        self.ptr.begin_frame(r, g, b, a)

    def end_frame(self):
        self.ptr.end_frame()

    def shutdown(self):
        self.ptr.shutdown()


cdef class PyShaderGL3:
    cdef gl3.ShaderGL3* ptr

    def __cinit__(self):
        self.ptr = new gl3.ShaderGL3()

    def load(self, vs: str, fs: str):
        cdef string cvs = vs.encode()
        cdef string cfs = fs.encode()
        return self.ptr.load(cvs, cfs)

    def bind(self):
        self.ptr.bind()

    def destroy(self):
        self.ptr.destroy()


cdef class PyMeshGL3:
    cdef gl3.MeshGL3* ptr

    def __cinit__(self):
        self.ptr = new gl3.MeshGL3()

    def init(self, vertices, indices):
        cdef int vcount = len(vertices)
        cdef int icount = len(indices)

        cdef float* vbuf = <float*>malloc(vcount * sizeof(float))
        cdef unsigned int* ibuf = <unsigned int*>malloc(icount * sizeof(unsigned int))

        cdef int i

        for i in range(vcount):
            vbuf[i] = float(vertices[i])

        for i in range(icount):
            ibuf[i] = <unsigned int>indices[i]

        ok = self.ptr.init(vbuf, vcount, ibuf, icount)

        free(vbuf)
        free(ibuf)

        return ok

    def draw(self):
        self.ptr.draw()

    def destroy(self):
        self.ptr.destroy()
