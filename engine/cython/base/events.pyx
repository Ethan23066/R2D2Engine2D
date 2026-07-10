# distutils: language = c++

from .events cimport Event, EventQueue, EventType

cdef class Events:
    cdef EventQueue q

    def __cinit__(self):
        self.q = EventQueue()

    def push(self, int type, int a=0, int b=0):
        cdef Event e = Event(<EventType>type, a, b)
        self.q.push(e)

    def poll(self):
        cdef Event out
        if self.q.poll(out):
            return (out.type, out.a, out.b)
        return None

    def clear(self):
        self.q.clear()
