from engine.cython.base.log cimport Log

cdef extern from "engine/core/base/log.hpp":
    cdef cppclass Log:
        @staticmethod
        void info(const char*)

cdef extern from "engine/core/base/manager.hpp":
    cdef cppclass RendererManager:
        RendererManager()
        bool init()
        void resize(int w, int h)
        void begin_frame()
        void end_frame()
        void shutdown()
