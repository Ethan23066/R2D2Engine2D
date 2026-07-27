# Exposition du C++ natif
cdef extern from "RendererGL3.hpp":
    cdef cppclass RendererGL3:
        RendererGL3()
        int init(void* win, int w, int h)
        void begin_frame(float r, float g, float b, float a)
        void end_frame()
        void shutdown()

# Exposition de la classe Cython
cdef class PyRendererGL3:
    cdef RendererGL3* cpp_renderer
