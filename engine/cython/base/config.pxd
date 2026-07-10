# distutils: language = c++

from libcpp.string cimport string

cdef extern from "engine/core/base/config.hpp" namespace "r2d2":
    cdef struct EngineConfig:
        int window_width
        int window_height
        bint vsync
        bint fullscreen
        string title

    void load_default_config(EngineConfig& cfg)
