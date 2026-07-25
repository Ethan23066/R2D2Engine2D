cdef extern from "RendererGL3.hpp":
    cdef cppclass RendererGL3:
        RendererGL3()
        bool init(void* window, int w, int h)
        void begin_frame(float r, float g, float b, float a)
        void end_frame()
        void shutdown()

cdef extern from "MeshGL3.hpp":
    cdef cppclass MeshGL3:
        MeshGL3()
        void upload()
        void draw()

cdef extern from "ShaderGL3.hpp":
    cdef cppclass ShaderGL3:
        ShaderGL3()
        bool load(const char* vs_path, const char* fs_path)
        void use()

cdef extern from "Texture.hpp":
    cdef cppclass Texture:
        Texture()
        bool load(const char* path)
        void bind(int unit)
