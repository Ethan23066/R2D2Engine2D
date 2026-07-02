from libcpp.string cimport string
from .time cimport Time as CTime

cdef class PyTime:
    cdef CTime* _c

    def __cinit__(self):
        self._c = new CTime()

    def update(self):
        self._c.update()

    def delta(self):
        return self._c.delta()

    def now(self):
        return self._c.now()
