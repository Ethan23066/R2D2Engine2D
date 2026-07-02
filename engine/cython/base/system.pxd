cdef extern from "system.hpp" namespace "r2d2":
    cdef cppclass System:
        @staticmethod
        void init()
        @staticmethod
        void shutdown()
        @staticmethod
        bint is_initialized()
