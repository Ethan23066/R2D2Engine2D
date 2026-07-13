cdef extern from "engine/core/backend/gl3/RendererGL3.hpp":
    cdef cppclass RendererGL3:
        RendererGL3()
        bool init()
        void resize(int w, int h)
        void begin_frame()
        void end_frame()
        void shutdown()
