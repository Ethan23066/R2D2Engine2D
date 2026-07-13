# distutils: language = c++

from libc.stdint cimport uint8_t

cdef extern from "engine/core/base/lifecycle.hpp" namespace "r2d2":
    cdef cppclass Lifecycle:
        Lifecycle() except +
        void init()
        void update()
        void shutdown()
        uint8_t is_running() const
