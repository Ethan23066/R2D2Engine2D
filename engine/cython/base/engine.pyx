# engine/cython/base/engine.pyx

from engine.cython.base.engine cimport Engine as CppEngine

cdef class PyEngine:
    cdef CppEngine* cpp

    def __cinit__(self):
        self.cpp = new CppEngine()

    def __dealloc__(self):
        del self.cpp

    def init(self):
        self.cpp.init()

    def shutdown(self):
        self.cpp.shutdown()

    def tick(self):
        self.cpp.tick()

    def is_running(self):
        return self.cpp.is_running()
