# distutils: language = c++

from libcpp.queue cimport queue
from libcpp cimport bool

cdef extern from "engine/core/base/events.hpp" namespace "r2d2":

    cdef enum EventType:
        NONE
        KEY_DOWN
        KEY_UP
        MOUSE_MOVE
        MOUSE_BUTTON_DOWN
        MOUSE_BUTTON_UP
        WINDOW_CLOSE

    cdef struct Event:
        EventType type
        int a
        int b

    cdef cppclass EventQueue:
        EventQueue() except +
        void push(const Event& e)
        bool poll(Event& out)
        void clear()
