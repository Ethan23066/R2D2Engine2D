from libcpp.string cimport string

cdef extern from "log.hpp" namespace "r2d2":
    cdef cppclass Log:
        @staticmethod
        void info(const char* msg)
        @staticmethod
        void warn(const char* msg)
        @staticmethod
        void error(const char* msg)
