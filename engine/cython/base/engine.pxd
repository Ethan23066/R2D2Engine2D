# engine/cython/base/engine.pxd

cdef extern from "engine.hpp" namespace "r2d2":
    cdef cppclass Engine:
        Engine()
        void init()
        void shutdown()
        void tick()
        bool is_running()
