from libcpp.string cimport string

cdef extern from "log.hpp" namespace "r2d2":
    cdef cppclass Log:
        @staticmethod
        void info(const string& msg)
        @staticmethod
        void warn(const string& msg)
        @staticmethod
        void error(const string& msg)
