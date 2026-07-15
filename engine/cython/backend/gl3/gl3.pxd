# engine/cython/backend/gl3/gl3.pxd

from libcpp.string cimport string

cdef extern from "RendererGL3.hpp" namespace "":
    cdef cppclass RendererGL3:
        RendererGL3()
        bool init(void* window, int w, int h)
        void begin_frame(float r, float g, float b, float a)
        void end_frame()
        void shutdown()

cdef extern from "MeshGL3.hpp" namespace "":
    cdef cppclass MeshGL3:
        MeshGL3()
        bool init(const float* vertices, int vcount,
                  const unsigned int* indices, int icount)
        void draw() const
        void destroy()

cdef extern from "ShaderGL3.hpp" namespace "":
    cdef cppclass ShaderGL3:
        ShaderGL3()
        bool load(const string& vs, const string& fs)
        void bind() const
        void destroy()
