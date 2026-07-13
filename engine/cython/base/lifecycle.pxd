# distutils: language = c++

cdef extern from "engine/core/base/lifecycle.hpp" namespace "r2d2":

    cdef cppclass Lifecycle:
        Lifecycle() except +
        void init()
        void update()
        void shutdown()
        bool is_running() const
