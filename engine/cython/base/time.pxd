cdef extern from "time.hpp" namespace "r2d2":
    cdef cppclass Time:
        Time()
        void update()
        double delta() const
        double now() const
