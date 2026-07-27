from engine.cython.backend.gl3.gl3 cimport PyRendererGL3

cdef class BackendManager:
    cdef PyRendererGL3 renderer
