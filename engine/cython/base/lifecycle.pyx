# distutils: language = c++

from .lifecycle cimport Lifecycle

cdef class PyLifecycle:
    cdef Lifecycle* lc

    def __cinit__(self):
        self.lc = new Lifecycle()

    def __dealloc__(self):
        if self.lc is not NULL:
            del self.lc
            self.lc = NULL

    def init(self):
        self.lc.init()

    def update(self):
        self.lc.update()

    def shutdown(self):
        self.lc.shutdown()

    def is_running(self):
        return bool(self.lc.is_running())
